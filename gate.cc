#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <memory>
#include <map>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <cerrno>
#ifndef WIN32
# include <sys/wait.h>
#endif
#ifdef WIN32
# include <process.h>
#endif
#include <typeinfo>
#include "xchat.h"
#include "irc.h"
#include "TomiTCP/net.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"
#include "setproctitle.h"
using namespace std;
using namespace xchat;
using namespace net;

#define VERSION "SVN-" REVISION

/*
 * Some global variables
 */
TomiTCP s;
string logfile;
string restrictfile;

/*
 * Constants
 */
static const char * const me = "xchat.cz";

/*
 * Make host
 */
inline string host(const x_nick &n)
{
    static const char * const sexhost[] = {
	"girls.xchat.cz",
	"boys.xchat.cz",
	"users.xchat.cz"
    };

    return strtolower_nr(n.nick) + "." + sexhost[n.sex];
}

/*
 * Make username
 */
inline string username(const x_nick &n) {
    return strtolower_nr(n.nick);
}

/*
 * Make nick!username@host
 */
inline string mask(const x_nick &n)
{
    return n.nick + "!" + username(n) + "@" + host(n);
}

/*
 * Make star
 */
inline string star(int star)
{
    static const char * const stars[] = {
	"Blue",
	"Green",
	"Yellow",
	"Red",
	"Black",
    };

    return stars[star - 1] + string(" star");
}

/*
 * Is it a notice from bttrw?
 */
bool is_notice(string &s)
{
    static string pat = "Notice: ";
    if (!s.compare(0, pat.length(), pat)) {
	s.erase(0, pat.length());
	return true;
    }

    return false;
}

void log(const string& s)
{
    if (logfile.length()) {
	time_t t = time(0);
	struct tm lt;
	char st[128];

	struct tm *plt = localtime_r(&t, &lt);
	if (!plt)
	    throw runtime_error(strerror(errno));

	strftime(st, 128, "%F %H:%M:%S", &lt);

	FILE *f = fopen(logfile.c_str(), "a");
	if (f) {
	    fprintf(f, "%s [%i] - %s\n", st, getpid(), s.c_str());
	    fclose(f);
	}
    } else {
	cout << s << endl;
    }
}

bool check_restrict(string nick)
{
    strtolower(nick);

    ifstream f(restrictfile.c_str());
    string l;

    while (f >> l) {
	if (nick == strtolower_nr(l))
	    return true;
    }

    return false;
}

#ifndef WIN32
pid_t parent = 0;

void sigchld(int) {
    int status, serrno;
    serrno = errno;
    while (1)
	if (waitpid (WAIT_ANY, &status, WNOHANG) <= 0)
	    break;
    errno = serrno;
}

void sighup(int) {
    /*
     * Close parent process and leave clients' processes
     */
    if (getpid() == parent) {
	exit(0);
    }

    close(0);
    close(1);
    close(2);
}
#endif

void welcome()
{
    cout << "Hello, this is an IRC to xchat gateway from xchat-bttrw r." REVISION << endl;
    cout << "Built with TomiTCP r." TOMITCP_REV << endl;
    cout << "Coded by Tomas Janousek <tomi@nomi.cz> and others" << endl;
    cout << "Homepage: http://nomi.cz/projects.shtml?id=xchat-bttrw" << endl;
#ifdef WIN32
    cout << "This is a Win32 build. " << endl;
#endif
    cout << endl;
    cout << "This is free software; see COPYRIGHT for copying conditions.  There is NO" << endl;
    cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
    cout << endl;
    cout << "Listening for incoming connections on " << tomi_ntop(s.lname) << "/" <<
	ntohs(PORT_SOCKADDR(s.lname)) << endl;
}

void reload_voiced_girls(auto_ptr<XChat> &x, auto_ptr<TomiTCP> &c,
	bool voiced_girls)
{
    string tmp;	
    nicklist_t::iterator j;
    int k;
    
    for (rooms_t::iterator i = x->rooms.begin();
	    i != x->rooms.end(); i++) {
	for (k = 1, tmp.clear(), j = i->second.nicklist.begin();
		j != i->second.nicklist.end(); j++)
	    if (j->second.sex == 0) {
		if (tmp.length())
		    tmp += " ";
		tmp += j->first;
		if (k % 5 == 0) {
    		    fprintf(*c, ":%s MODE #%s %svvvvv %s\n", me,
    			    i->first.c_str(), (voiced_girls?"+":"-"),
			    tmp.c_str());
		    tmp.clear();
		}
		k++;
	    }
	if (tmp.length())
	    fprintf(*c, ":%s MODE #%s %svvvvv %s\n", me,
    		    i->first.c_str(), (voiced_girls?"+":"-"),
		    tmp.c_str());
    }
}

void serve_client(TomiTCP *cptr)
{
    net::thread_init();

    auto_ptr<TomiTCP> c(cptr);
    auto_ptr<XChat> x;
    time_t last_ping = 0, last_ping_sent = 0, connect_time = time(0);
    bool voiced_girls = false, show_advert = true, show_date = false,
	 show_idler = true;
    bool user = false;
    string nick, pass;

    setproctitle(("gate: " + tomi_ntop(c->rname)).c_str());
    log(tomi_ntop(c->rname) + " - Connected");

    try {
	while (1) {
	    if (input_timeout(c->netsock, 1000) > 0) {
		/*
		 * Got message from IRC client
		 */
		string l, prefix;
		vector<string> cmd;
		if (!c->getline(l))
		    break;
	       	chomp(l);

		parsein(l, prefix, cmd);
		if (!cmd.size()) continue;

		strtoupper(cmd[0]);

		//cout << l << endl;

		/*
		 * User registration
		 */
		if (!x.get()) {
		    if (cmd[0] == "NICK" && cmd.size() >= 2) {
			/*
			 * Check nickname
			 */
			if (cmd[1].length() > 20) {
			    fprintf(*c, ":%s 432 * %s :Erroneous Nickname\n", me,
				    cmd[1].c_str());
			    break;
			}

			bool err = false;
			for (string::iterator i = cmd[1].begin(); i != cmd[1].end(); i++)
			    if (!isprint(*i)) {
				err = true;
				break;
			    }
			if (err) {
			    fprintf(*c, ":%s 432 * %s :Erroneous Nickname\n", me,
				    cmd[1].c_str());
			    break;
			}

			nick = cmd[1];
		    } else if (cmd[0] == "PASS" && cmd.size() >= 2) {
			pass = cmd[1];
		    } else if (cmd[0] == "USER" && cmd.size() > 1) {
			user = true;
		    } else {
			/*
			 * If the user is not registered and sends some other
			 * command, quit him.
			 */
			fprintf(*c, ":%s ERROR :Not registered\n", me);
			break;
		    }
		    
		    if (user && nick.length()) {
			if (!pass.length()) {
			    fprintf(*c, ":%s NOTICE AUTH :Need password!\n", me);
			    continue;
			}

			if (restrictfile.length() && !check_restrict(nick)) {
			    fprintf(*c, ":%s ERROR :Unauthorized user!\n", me);
			    log(tomi_ntop(c->rname) + " - Unauthorized - " + nick);
			    break;
			}

			log(tomi_ntop(c->rname) + " - Logging in - " + nick);

			setproctitle(("gate: " + tomi_ntop(c->rname) + " (" + nick + ")").c_str());

			try { x.reset(new XChat(nick, pass)); }
			catch (runtime_error &e) {
			    fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			    break;
			}

			log(tomi_ntop(c->rname) + " - Logged in - " + nick);

			/*
			 * Successful login, so output some numerics to make
			 * client happy.
			 */

			fprintf(*c, ":%s 001 %s :Welcome Back To The Real World,"
				" but still connected to xchat.cz\n", me, nick.c_str());
			fprintf(*c, ":%s 002 %s :Your host is %s[%s/%i]"
				", running version xchat-bttrw " VERSION "\n", me,
				nick.c_str(), me, revers(c->lname).c_str(),
				ntohs(PORT_SOCKADDR(s.lname)));
			fprintf(*c, ":%s 003 %s :This server was created god knows when\n",
				me, nick.c_str());
			fprintf(*c, ":%s 004 %s :%s xchat-bttrw-" VERSION " 0 io\n",
				me, nick.c_str(), me);
			fprintf(*c, ":%s 005 %s MODES=5 MAXTARGETS=1 NICKLEN=20 SAFELIST"
				" :are supported by this server\n",
				me, nick.c_str());
			fprintf(*c, ":%s 005 %s CHANTYPES=# PREFIX=(aohv)!@%%+ CHANMODES=,,,i"
				" NETWORK=xchat.cz CASEMAPPING=ascii"
				" :are supported by this server\n", me, nick.c_str());

			fprintf(*c, ":%s 375 %s :- %s Message of the Day -\n", me,
				nick.c_str(), me);

			ifstream copyright("COPYRIGHT");
			if (copyright) {
			    string motd;
			    while (getline(copyright, motd))
				fprintf(*c, ":%s 372 %s :%s\n", me, nick.c_str(),
					motd.c_str());
			}

			fprintf(*c, ":%s 376 %s :Web login: %s\n",
			    me, nick.c_str(),
			    x->makepath("", PATH_STATIC).c_str());

			fprintf(*c, ":%s 376 %s :End of /MOTD command.\n", me, nick.c_str());
		    }

		    continue;
		}

		/*
		 * And here follow commands, which can be invoked after
		 * successful registration.
		 */
		if (cmd[0] == "SET" && cmd.size() >= 2) {
		    strtoupper(cmd[1]);
		    if (cmd[1] == "IDLE_INTERVAL") {
			/*
			 * Note: You're violating the XChat rules by using
			 * this.
			 */
			if (cmd.size() == 3) {
			    x->idle_interval = atoi(cmd[2].c_str());
			    setproctitle(("gate: " + tomi_ntop(c->rname) +
				" (" + nick + ")" + (x->idle_interval>0?
				" [IDLER=" + cmd[2] + "]":"")).c_str());
			}
			fprintf(*c, ":%s NOTICE %s :idle_interval set to %i\n",
				me, nick.c_str(), x->idle_interval);
		    } else if (cmd[1] == "CHARSET") {
			if (cmd.size() == 3)
			    x->client_charset = cmd[2];
			fprintf(*c, ":%s NOTICE %s :client_charset set to %s\n",
				me, nick.c_str(), x->client_charset.c_str());
		    } else if (cmd[1] == "VOICED_GIRLS") {
			bool old_voiced_girls = voiced_girls;
			if (cmd.size() == 3)
			    voiced_girls = atoi(cmd[2].c_str());
			if (voiced_girls != old_voiced_girls)
			    reload_voiced_girls(x, c, voiced_girls);
			fprintf(*c, ":%s NOTICE %s :voiced_girls set to %i\n",
				me, nick.c_str(), voiced_girls);
		    } else if (cmd[1] == "SHOW_ADVERT") {
			/*
			 * Note that you're probably not behaving fair while
			 * having this turned off.
			 */
			if (cmd.size() == 3)
			    show_advert = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :show_advert set to %i\n",
				me, nick.c_str(), show_advert);
		    } else if (cmd[1] == "RECV_INTERVAL") {
			if (cmd.size() == 3)
			    x->recv_interval = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :recv_interval set to %i\n",
				me, nick.c_str(), x->recv_interval);
		    } else if (cmd[1] == "SHOW_DATE") {
			if (cmd.size() == 3)
			    show_date = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :show_date set to %i\n",
				me, nick.c_str(), show_date);
		    } else if (cmd[1] == "CONVERT_SMILES") {
			if (cmd.size() == 3)
			    x->convert_smiles = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :convert_smiles set to %i\n",
				me, nick.c_str(), x->convert_smiles);
		    } else if (cmd[1] == "REALLY_LOGOUT" && cmd.size() >= 2) {
			if (cmd.size() == 3)
			    x->really_logout = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :really_logout set to %i\n",
				me, nick.c_str(), x->really_logout);
		    } else if (cmd[1] == "SHOW_IDLER") {
			if (cmd.size() == 3)
			    show_idler = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :show_idler set to %i\n",
				me, nick.c_str(), show_idler);
		    } else if (cmd[1] == "WATCH_GLOBAL") {
			if (cmd.size() == 3)
			    x->watch_global = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :watch_global set to %i\n",
				me, nick.c_str(), x->watch_global);
		    } else if (cmd[1] == "SHOW_HISTORY") {
			if (cmd.size() == 3)
			    x->show_history = atoi(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :show_history set to %i\n",
				me, nick.c_str(), x->show_history);
		    } else {
			fprintf(*c, ":%s NOTICE %s :Bad variable or parameter"
				" count\n", me, nick.c_str());
		    }
		} else if (cmd[0] == "PING") {
		    if (cmd.size() >= 2) {
			fprintf(*c, ":%s PONG :%s\n", me, cmd[1].c_str());
		    } else {
			fprintf(*c, ":%s PONG %s\n", me, me);
		    }
		} else if (cmd[0] == "PONG" && cmd.size() >= 2 && cmd[1] == me) {
		    last_ping_sent = 0;
		} else if (cmd[0] == "QUIT") {
		    break;
		} else if (cmd[0] == "JOIN" && cmd.size() >= 2) {
		    stringstream s(cmd[1]);
		    string chan;

		    /*
		     * Join comma separated list of channels
		     */
		    while (getline(s, chan, ',')) {
			if (chan[0] == '#')
			    chan.erase(chan.begin());

			try {
			    x->join(chan);

			    fprintf(*c, ":%s JOIN #%s\n",
				    mask(x->me).c_str(),
				    chan.c_str());
			    fprintf(*c, ":%s 332 %s #%s :%s\n", me, nick.c_str(), chan.c_str(),
				    (x->rooms[chan].name + " | " +
				     x->rooms[chan].desc + " | " +
				     x->rooms[chan].web).c_str());
			    // output userlist (NAMES)
			    string tmp; int i; nicklist_t::iterator j;
			    for (i = 1, j = x->rooms[chan].nicklist.begin();
				    j != x->rooms[chan].nicklist.end(); j++, i++) {
				if (tmp.length())
				    tmp += " ";
				tmp += string("") +
				    ((x->issuperadmin(j->first))?"!":"") +
				    ((x->ispermadmin(chan, j->first))?"@":"") +
				    ((x->isadmin(chan, j->first))?"%":"") +
				    ((j->second.sex == 0 && voiced_girls)?"+":"") +
				    j->second.nick;
				if (i % 5 == 0) {
				    fprintf(*c, ":%s 353 %s = #%s :%s\n", me, nick.c_str(),
					    chan.c_str(), tmp.c_str());
				    tmp.clear();
				}
			    }
			    if (tmp.length()) {
				fprintf(*c, ":%s 353 %s = #%s :%s\n", me, nick.c_str(),
					chan.c_str(), tmp.c_str());
			    }
			    fprintf(*c, ":%s 366 %s #%s :End of /NAMES list.\n", me,
				    nick.c_str(), chan.c_str());

			    if (x->me.sex == 0 && voiced_girls)
				fprintf(*c, ":%s MODE #%s +v %s\n", me,
					chan.c_str(), nick.c_str());

			} catch (runtime_error &e) {
			    fprintf(*c, ":%s 403 %s #%s :Could not join channel\n",
				    me, nick.c_str(), chan.c_str());
			    fprintf(*c, ":%s NOTICE %s :Could not join #%s: %s\n", me,
				    nick.c_str(), chan.c_str(), e.what());
			}
		    }
		} else if (cmd[0] == "PART" && cmd.size() >= 2) {
		    stringstream s(cmd[1]);
		    string chan;

		    /*
		     * Part comma separated list of channels
		     */
		    while (getline(s, chan, ',')) {
			if (chan[0] == '#')
			    chan.erase(chan.begin());

			if (x->rooms.find(chan) != x->rooms.end()) {
			    try {
				x->leave(chan);
				fprintf(*c, ":%s PART #%s :\n",
					mask(x->me).c_str(),
					chan.c_str());
			    } catch (runtime_error &e) {
				fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
					nick.c_str(), e.what());
			    }
			} else {
			    fprintf(*c, ":%s 403 %s %s :No such channel\n", me,
				    nick.c_str(), chan.c_str());
			}
		    }
		} else if ((cmd[0] == "PRIVMSG" || cmd[0] == "NOTICE") && cmd.size() == 3) {
		    if (cmd[0] == "NOTICE") {
			static string ver = "\x01VERSION ";
			if (!cmd[2].compare(0, ver.length(), ver))
			    cmd[2].insert(ver.length(), "[xchat-bttrw r." REVISION
				    ", TomiTCP r." TOMITCP_REV "] ");

			cmd[2] = "Notice: " + cmd[2];
		    }

		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			/*
			 * Channel message
			 */

			if (x->rooms.find(cmd[1]) == x->rooms.end())
			    fprintf(*c, ":%s 404 %s %s :Cannot send to channel\n", me,
				    nick.c_str(), cmd[1].c_str());
			else
			    x->msg(cmd[1], cmd[2]);
		    } else {
			/*
			 * Private message
			 */
			try { x->whisper(cmd[1], cmd[2]); }
			catch (runtime_error &e) {
			    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				    nick.c_str(), e.what());
			}
		    }
		} else if (cmd[0] == "MODE" && cmd.size() == 2) {
		    /*
		     * Handle MODE request
		     */
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			if (x->rooms.find(cmd[1]) == x->rooms.end())
			    fprintf(*c, ":%s 403 %s #%s :No such channel\n", me, nick.c_str(),
				    cmd[1].c_str());
			else
			    fprintf(*c, ":%s 324 %s #%s +%s\n", me, nick.c_str(),
				    cmd[1].c_str(), (x->rooms[cmd[1]].locked)?"i":"");
		    } else {
			fprintf(*c, ":%s 221 %s +\n", me, nick.c_str());
		    }
		} else if (cmd[0] == "MODE" && cmd.size() >= 3 && cmd[1][0] == '#') {
		    /*
		     * Handle MODE command...
		     */
		    vector<pair<string, string> > modes;
		    parsemode(vector<string>(cmd.begin() + 2, cmd.end()), modes);

		    cmd[1].erase(cmd[1].begin());

		    // we don't want to set admin more than once
		    bool admin = 1;

		    for (vector<pair<string, string> >::iterator i = modes.begin();
			    i != modes.end(); i++) {
			if (i->first == "?b") {
			    // just to make client's `channel synchronizing' happy
			    fprintf(*c, ":%s 368 %s #%s :End of Channel Ban List\n", me,
				    nick.c_str(), cmd[1].c_str());
			} else if (i->first == "+o" || i->first == "+h") {
			    // +o just to maintain backward compatibility and
			    // ease of use
			    if (admin) {
				x->admin(cmd[1], i->second);
				admin = 0;
			    }
			} else if (i->first == "+i") {
			    x->lock(cmd[1]);
			} else if (i->first == "-i") {
			    x->unlock(cmd[1]);
			}
		    }
		} else if (cmd[0] == "WHO" && cmd.size() == 2) {
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			if (x->rooms.find(cmd[1]) != x->rooms.end()) {
			    /*
			     * Output channel WHO
			     */
			    for (nicklist_t::iterator i = x->rooms[cmd[1]].nicklist.begin();
				    i != x->rooms[cmd[1]].nicklist.end(); i++) {
				fprintf(*c, ":%s 352 %s #%s %s %s %s %s H%s%s%s%s :%d %s\n", me,
					nick.c_str(), cmd[1].c_str(),
					username(i->second).c_str(),
					host(i->second).c_str(),
					me, i->second.nick.c_str(),
					(x->issuperadmin(i->first))?"!*":"",
					(x->ispermadmin(cmd[1], i->first))?"@":"",
					(x->isadmin(cmd[1], i->first))?"%":"",
					(i->second.sex == 0 && voiced_girls)?"+":"",
					0, "xchat.cz user");
			    }
			}
			cmd[1] = "#" + cmd[1];
		    } else {
			/*
			 * Output user WHO
			 */
			x_nick *n = x->findnick(cmd[1], 0);
			if (n)
			    fprintf(*c, ":%s 352 %s %s %s %s %s %s %s%s :%d %s\n", me,
				    nick.c_str(), "*", username(*n).c_str(),
				    host(*n).c_str(), me, n->nick.c_str(), "H",
				    (x->issuperadmin(n->nick))?"*":"",
				    0,
				    "xchat.cz user");
		    }
		    fprintf(*c, ":%s 315 %s %s :End of /WHO list.\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "WHOIS" && cmd.size() >= 2) {
		    /*
		     * Output user WHOIS
		     */

                    try {
                        userinfo_t us = x->userinfo(cmd[1]);

                        if (!us.nick.length()) {
                            fprintf(*c, ":%s 401 %s %s :No such nick/channel\n", me,
                                    nick.c_str(), cmd[1].c_str());
                        } else {
                            x_nick n;
                            n.nick = us.nick;
                            n.sex = us.sex;
                            
                            string info = us.name;
                            
                            if (us.surname != "") {
                                if (info != "")
                                    info += " ";
                                info += us.surname;
                            }
                            
                            if (us.age != 0) {
                                if (info != "")
                                    info += " ";
                                info += "(" + tostr<int>(us.age) + ")";
                            }
                            
                            if (us.email != "") {
                                if (info != "")
                                    info += " ";
                                info += "(" + us.email + ")";
                            }
                            
                            if (info != "")
                                info += " ";
                            info += "last: " + us.last_online + ", ";
                            
                            info += "wasted: " +
                                tostr_float<double>(us.time_wasted / 3600.0, 2) +
                                " h, ";

                            info += "created: " + us.nick_created + ", ";

                            info += "TOP: " + us.top_pos;

                            if (us.icq != "") {
                                info += ", ICQ: " + us.icq;
                            }

                            fprintf(*c, ":%s 311 %s %s %s %s * :%s\n", me,
                                    nick.c_str(), n.nick.c_str(),
                                    username(n).c_str(), host(n).c_str(),
                                    info.c_str());

                            if (us.rooms.size()!=0) {
                                string rooms = "";
                                int ii = 1;
                                for (vector<userinfo_room>::iterator i = us.rooms.begin();
                                        i != us.rooms.end(); i++, ii++) {
                                    if (rooms.length())
                                        rooms += " ";
                                    string name = i->name;
                                    for (string::iterator j = name.begin(); j != name.end(); j++)
                                        if (*j == ' ')
                                            *j = '_';
                                    rooms += "#" + i->rid + "(" + name + "-" + i->idle + ")";

                                    if (ii % 5 == 0) {
                                        fprintf(*c, ":%s 319 %s %s :%s\n", me,
                                                nick.c_str(), n.nick.c_str(),
                                                rooms.c_str());
                                        rooms.clear();
                                    }
                                }
                                if (rooms.length()) {
                                    fprintf(*c, ":%s 319 %s %s :%s\n", me,
                                            nick.c_str(), n.nick.c_str(),
                                            rooms.c_str());
                                }
                            }
                            if (us.star || us.cert)
                                fprintf(*c, ":%s 313 %s %s :is an %s%s%s\n", me,
                                        nick.c_str(), n.nick.c_str(),
                                        (us.star)?star(us.star).c_str():"",
                                        (us.star && us.cert)?", ":"",
                                        (us.cert)?"certified":"");
                        }
                        fprintf(*c, ":%s 318 %s %s :End of /WHOIS list.\n", me,
                                nick.c_str(), (us.nick.length() ?
                                    us.nick : cmd[1]).c_str());
		    } catch (runtime_error &e) {
			fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				nick.c_str(), e.what());
		    }
		} else if (cmd[0] == "KICK" && cmd.size() >= 3) {
		    /*
		     * Kick user.
		     */
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());
		    }

		    x->kick(cmd[1], cmd[2], (cmd.size() > 3)?cmd[3]:"");
		} else if (cmd[0] == "KILL" && cmd.size() >= 2) {
		    /*
		     * Kill user.
		     */
		    try {
			x->kill(cmd[1], (cmd.size() > 2)?cmd[2]:"");
		    } catch (runtime_error &e) {
			fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				nick.c_str(), e.what());
		    }
		} else if (cmd[0] == "TOPIC" && cmd.size() == 2) {
		    /*
		     * Output topic
		     */
		    if (cmd[1][0] == '#')
			cmd[1].erase(cmd[1].begin());

		    if (x->rooms.find(cmd[1]) == x->rooms.end())
			fprintf(*c, ":%s 403 %s #%s :No such channel\n", me, nick.c_str(),
				cmd[1].c_str());
		    else
			fprintf(*c, ":%s 332 %s #%s :%s\n", me, nick.c_str(), cmd[1].c_str(),
				(x->rooms[cmd[1]].name + " | " + 
				 x->rooms[cmd[1]].desc + " | " +
				 x->rooms[cmd[1]].web).c_str());
		} else if (cmd[0] == "TOPIC" && cmd.size() >= 3) {
		    /*
		     * Set topic
		     */
		    if (cmd[1][0] == '#')
			cmd[1].erase(cmd[1].begin());

		    try { x->setdesc(cmd[1], cmd[2]); }
		    catch (runtime_error &e) {
			fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				nick.c_str(), e.what());
		    }
		} else if (cmd[0] == "NAMES" && cmd.size() == 2) {
		    /*
		     * Output names reply
		     */
		    if (cmd[1][0] == '#')
			cmd[1].erase(cmd[1].begin());

		    if (x->rooms.find(cmd[1]) == x->rooms.end())
			fprintf(*c, ":%s 403 %s #%s :No such channel\n", me, nick.c_str(),
				cmd[1].c_str());
		    else {
			string tmp; int i; nicklist_t::iterator j;
			for (i = 1, j = x->rooms[cmd[1]].nicklist.begin();
				j != x->rooms[cmd[1]].nicklist.end(); j++, i++) {
			    tmp += string("") +
				((x->issuperadmin(j->first))?"!":"") +
				((x->ispermadmin(cmd[1], j->first))?"@":"") +
				((x->isadmin(cmd[1], j->first))?"%":"") +
				((j->second.sex == 0 && voiced_girls)?"+":"") +
				j->second.nick + " ";
			    if (i % 5 == 0) {
				fprintf(*c, ":%s 353 %s = #%s :%s\n", me, nick.c_str(),
					cmd[1].c_str(), tmp.c_str());
				tmp.clear();
			    }
			}
			if (tmp.length()) {
			    fprintf(*c, ":%s 353 %s = #%s :%s\n", me, nick.c_str(),
				    cmd[1].c_str(), tmp.c_str());
			}
			fprintf(*c, ":%s 366 %s #%s :End of /NAMES list.\n", me,
				nick.c_str(), cmd[1].c_str());
		    }
		} else if (cmd[0] == "LIST" && cmd.size() >= 1) {
		    /*
		     * Get a list of rooms
		     */
		    try {
			listout_t listout;
			int type = ROOM_REGISTERED + ROOM_TEMPORARY;
			
			if (cmd.size() > 1) {
			    strtoupper(cmd[1]);
			    if (cmd[1] == "R")
				type = ROOM_REGISTERED;
			    else if (cmd[1] == "T")
				type = ROOM_TEMPORARY;
			}
			
			x->list(listout, type);
			
			for (listout_t::iterator i = listout.begin();
				i != listout.end(); i++) {
			    fprintf(*c, ":%s 322 %s #%s %i :%s\n", me, nick.c_str(),
				    i->rid.c_str(), i->count, (i->name +
					(i->registered ? " [R]" : "")).c_str());
			}
			fprintf(*c, ":%s 323 %s :End of /LIST\n", me, nick.c_str());
		    } catch (runtime_error &e) {
			fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				nick.c_str(), e.what());
		    }
		} else if (cmd[0] == "USERHOST" && cmd.size() >= 2) {
		    /*
		     * Output a USERHOST reply
		     */
		    string rpl;
		    for (vector<string>::iterator i = cmd.begin() + 1;
			    i != cmd.end(); i++) {
			x_nick *n = x->findnick(*i, 0);
			if (n)
			    rpl += n->nick + "=+" +
				username(*n) + "@" + host(*n) + " ";
		    }
		    fprintf(*c, ":%s 302 %s :%s\n", me, nick.c_str(), rpl.c_str());
		} else if (cmd[0] == "ISON" && cmd.size() >= 2) {
		    /*
		     * Output an ISON reply
		     */
                    try {
                        string rpl;
                        for (vector<string>::iterator i = cmd.begin() + 1;
                                i != cmd.end(); i++) {
                            string n;
                            stringstream sn(*i);
                            while (sn >> n)
                                if (x->ison(n))
                                    rpl += n + " ";
                        }
                        fprintf(*c, ":%s 303 %s :%s\n", me, nick.c_str(), rpl.c_str());
		    } catch (runtime_error &e) {
			fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				nick.c_str(), e.what());
		    }
		} else if (cmd[0] == "STATS" && cmd.size() >= 2) {
                    try {
                        switch (cmd[1][0]) {
                            case 'o':
                            case 'O':
                                x->reloadsuperadmins();
                                for (superadmins_t::iterator i = x->superadmins.begin();
                                    i != x->superadmins.end(); i++) {
                                    fprintf(*c, ":%s 243 %s %c * * %s (%s)\n", me, nick.c_str(),
                                        cmd[1][0], i->second.nick.c_str(), star(i->second.star).c_str());
                                }
                                fprintf(*c, ":%s 219 %s %c :End of STATS report\n", me,
                                    nick.c_str(), cmd[1][0]);
                                break;
                            case 'p':
                                x->reloadsuperadmins();
                                for (superadmins_t::iterator i = x->superadmins.begin();
                                    i != x->superadmins.end(); i++) {
                                    if (i->second.online) {
                                        fprintf(*c, ":%s 249 %s p %s (%s)\n", me, nick.c_str(),
                                            i->second.nick.c_str(), star(i->second.star).c_str());
                                    }
                                }
                                fprintf(*c, ":%s 219 %s p :End of STATS report\n", me,
                                    nick.c_str());
                                break;
                            default:
                                fprintf(*c, ":%s NOTICE %s :Bad parameter\n", me,
                                    nick.c_str());
                                break;
                        }
		    } catch (runtime_error &e) {
			fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				nick.c_str(), e.what());
		    }
		} else if (cmd[0] == "MAP") {
		    for (vector<server>::iterator i = x->servers.begin();
				i != x->servers.end(); i++) {
    			string last_break;
			
			if (i->last_break) {
			    char lb[128];
			    struct tm lt;
    			    struct tm *plt = localtime_r(&i->last_break, &lt);
    			    if (!plt)
		        	throw runtime_error(strerror(errno));
    			    strftime(lb, 128, "%F %H:%M:%S", &lt);
			    
			    last_break = " | Last break: " + string(lb) + " (" +
				tostr<int>(time(0) - i->last_break) + " sec)";
			}

                        string hosts;
                        for (server::types_t::iterator j = i->types.begin();
                                j != i->types.end(); j++) {
                            if (!hosts.empty())
                                hosts += ", ";
                            hosts += j->second;
                        }
			
			fprintf(*c, ":%s 015 %s :%s | Breaks: %d (%d total)%s%s | Vhosts: %s\n",
			    me, nick.c_str(),
			    tomi_ntop(i->host).c_str(), i->break_count,
			    i->total_break_count, last_break.c_str(),
			    (i->break_count >= tries_to_rest)?" | [REST]":"",
                            hosts.c_str());
		    }
		    fprintf(*c, ":%s 017 %s :End of /MAP\n",
			me, nick.c_str());
		} else if (cmd[0] == "AWAY") {
		    if (cmd.size() == 2 && cmd[1] != "")
			fprintf(*c, ":%s 306 %s :You have been marked as being away\n",
			        me, nick.c_str());
		    else 
			fprintf(*c, ":%s 305 %s :You are no longer marked as being away\n",
			        me, nick.c_str());
		} else {
		    log(tomi_ntop(c->rname) + " - Unknown command (" + l + ")");
		    fprintf(*c, ":%s 421 %s %s :Unknown command\n", me, nick.c_str(),
			    cmd[0].c_str());
		}
	    }

	    if (x.get()) {
		/*
		 * Let x do it's job on send queue
		 */
		try { x->do_sendq(); }
		catch (runtime_error &e) {
		    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
			    nick.c_str(), e.what());
		}

		/*
		 * Receive some data sometimes
		 */
		try { x->fill_recvq(); }
		catch (runtime_error &e) {
		    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
			    nick.c_str(), e.what());
		}
	    }

	    /*
	     * Go through recv queue and process messages
	     */
	    while (x.get() && !x->recvq.empty()) {
		auto_ptr<Event> e(x->recvq_pop());

		string date;
		if (show_date && e->date().length())
		    date = "\x01" "DATE " + e->date() + "\x01";

		if (dynamic_cast<EvRoomError*>(e.get())) {
		    auto_ptr<EvRoomError> f((EvRoomError*)e.release());

		    if (f->isfatal()) {
			fprintf(*c, ":%s KICK #%s %s :Error: %s\n", me,
				f->getrid().c_str(), nick.c_str(),
				f->str().c_str());
		    } else {
			fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				nick.c_str(), f->str().c_str());
		    }
		} else if (dynamic_cast<EvRoomMsg*>(e.get())) {
		    auto_ptr<EvRoomMsg> f((EvRoomMsg*)e.release());
		    string str = f->str();
		    bool notice = is_notice(str);

		    fprintf(*c, ":%s %s #%s :%s%s\n",
			    mask(f->getsrc()).c_str(),
			    notice?"NOTICE":"PRIVMSG", f->getrid().c_str(),
			    date.c_str(), str.c_str());
		} else if (dynamic_cast<EvWhisper*>(e.get())) {
		    auto_ptr<EvWhisper> f((EvWhisper*)e.release());
		    string str = f->str();
		    bool notice = is_notice(str);

		    fprintf(*c, ":%s %s %s :%s%s\n",
			    mask(f->getsrc()).c_str(),
			    notice?"NOTICE":"PRIVMSG", f->gettarget().nick.c_str(),
			    date.c_str(), str.c_str());
		} else if (dynamic_cast<EvRoomJoin*>(e.get())) {
		    auto_ptr<EvRoomJoin> f((EvRoomJoin*)e.release());

		    fprintf(*c, ":%s JOIN #%s\n",
			    mask(f->getsrc()).c_str(),
			    f->getrid().c_str());

		    string mode;
		    if (x->issuperadmin(f->getsrc().nick))
			mode += "a";
		    if (x->ispermadmin(f->getrid(), f->getsrc().nick))
			mode += "o";
		    if (f->getsrc().sex == 0 && voiced_girls)
			mode += "v";

		    if (mode.length())
			fprintf (*c, ":%s MODE #%s +%s %s%s%s\n",
				me, f->getrid().c_str(), mode.c_str(),
				f->getsrc().nick.c_str(), ((mode.length() == 2)?" ":""),
				((mode.length() == 2)?f->getsrc().nick.c_str():""));
		    
		} else if (dynamic_cast<EvRoomLeave*>(e.get())) {
		    auto_ptr<EvRoomLeave> f((EvRoomLeave*)e.release());

		    fprintf(*c, ":%s PART #%s :%s\n",
			    mask(f->getsrc()).c_str(),
			    f->getrid().c_str(), f->getreason().c_str());
		} else if (dynamic_cast<EvRoomKick*>(e.get())) {
		    auto_ptr<EvRoomKick> f((EvRoomKick*)e.release());

		    fprintf(*c, ":%s KICK #%s %s :%s\n",
			    mask(f->getsrc()).c_str(),
			    f->getrid().c_str(), f->gettarget().nick.c_str(),
			    f->getreason().c_str());
		} else if (dynamic_cast<EvRoomSysMsg*>(e.get())) {
		    auto_ptr<EvRoomSysMsg> f((EvRoomSysMsg*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :%sSystem: %s\n", me,
			    f->getrid().c_str(), date.c_str(), f->str().c_str());
		} else if (dynamic_cast<EvRoomIdlerMsg*>(e.get())) {
		    auto_ptr<EvRoomIdlerMsg> f((EvRoomIdlerMsg*)e.release());

		    if (show_idler) {
			fprintf(*c, ":%s NOTICE #%s :%sSystem: %s [IDLER]\n", me,
				f->getrid().c_str(), date.c_str(),
				f->str().c_str());
		    }
		} else if (dynamic_cast<EvRoomHistoryMsg*>(e.get())) {
		    auto_ptr<EvRoomHistoryMsg> f((EvRoomHistoryMsg*)e.release());

		    string prefix = f->getsrc();
		    if (f->gettarget().length())
			prefix += "->" + f->gettarget();

		    string date2;
		    if (f->date().length())
			date2 = "[" + f->date() + "] ";

		    fprintf(*c, ":%s NOTICE #%s :%s%s\002<%s>\002 %s\n", me,
		    	f->getrid().c_str(), date.c_str(), date2.c_str(),
			prefix.c_str(), f->str().c_str());
		} else if (dynamic_cast<EvRoomSysText*>(e.get())) {
		    auto_ptr<EvRoomSysText> f((EvRoomSysText*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :%s%s\n", me,
		    	f->getrid().c_str(), date.c_str(), f->str().c_str());
		} else if (dynamic_cast<EvRoomAdminChange*>(e.get())) {
		    auto_ptr<EvRoomAdminChange> f((EvRoomAdminChange*)e.release());

		    /*
		     * Get the right case for nicks, avoids mirc setting nick
		     * to lowercase in nicklist.
		     */
		    x_nick before = { f->getbefore(), 2 },
			   now = { f->getnow(), 2 },
			   *tmp;
		    if (before.nick.length() && (tmp = x->findnick(before.nick, 0)))
			before = *tmp;
		    if (now.nick.length() && (tmp = x->findnick(now.nick, 0)))
			now = *tmp;

		    bool minus = !before.nick.empty() &&
			x->isin(f->getrid(), before.nick) &&
			!x->ispermadmin(f->getrid(), before.nick);
		    bool plus = !now.nick.empty() &&
			x->isin(f->getrid(), now.nick) &&
			!x->ispermadmin(f->getrid(), now.nick);

		    if (minus && plus)
			fprintf(*c, ":%s MODE #%s -h+h %s %s\n", me,
				f->getrid().c_str(), before.nick.c_str(),
				now.nick.c_str());
		    else if (plus)
			fprintf(*c, ":%s MODE #%s +h %s\n", me,
				f->getrid().c_str(), now.nick.c_str());
		    else if (minus)
			fprintf(*c, ":%s MODE #%s -h %s\n", me,
				f->getrid().c_str(), before.nick.c_str());
		} else if (dynamic_cast<EvRoomAdminsChange*>(e.get())) {
		    auto_ptr<EvRoomAdminsChange> f((EvRoomAdminsChange*)e.release());

		    map<string, room>::iterator r = x->rooms.find(f->getrid());
		    if (r != x->rooms.end()) {
			nicklist_t::iterator n;

			for (vector<string>::const_iterator i = f->getadded().begin();
				i != f->getadded().end(); i++)
			    if ((n = r->second.nicklist.find(*i)) != r->second.nicklist.end())
				fprintf(*c, ":%s MODE #%s +o %s\n", me,
					f->getrid().c_str(), n->second.nick.c_str());

			for (vector<string>::const_iterator i = f->getremoved().begin();
				i != f->getremoved().end(); i++)
			    if ((n = r->second.nicklist.find(*i)) != r->second.nicklist.end()
				    && !x->isadmin(f->getrid(), *i))
				fprintf(*c, ":%s MODE #%s -o %s\n", me,
					f->getrid().c_str(), n->second.nick.c_str());
		    }
		} else if (dynamic_cast<EvRoomLockChange*>(e.get())) {
		    auto_ptr<EvRoomLockChange> f((EvRoomLockChange*)e.release());

		    fprintf(*c, ":%s MODE #%s %si\n", me,
			    f->getrid().c_str(), (f->getnow())?"+":"-");
		} else if (dynamic_cast<EvRoomAdvert*>(e.get())) {
		    auto_ptr<EvRoomAdvert> f((EvRoomAdvert*)e.release());

		    if (show_advert)
			fprintf(*c, ":%s NOTICE #%s :%sAdvert: %s [ %s ]\n", me,
				f->getrid().c_str(), date.c_str(), f->str().c_str(),
				f->getlink().c_str());
		} else if (dynamic_cast<EvRoomTopicChange*>(e.get())) {
		    auto_ptr<EvRoomTopicChange> f((EvRoomTopicChange*)e.release());
		
		    fprintf(*c, ":%s TOPIC #%s :%s\n", me, f->getrid().c_str(),
    			(f->getname() + " | " + f->getdesc() + " | " + 
			 f->getweb()).c_str());
		} else if (dynamic_cast<EvRoomOther*>(e.get())) {
		    auto_ptr<EvRoomOther> f((EvRoomOther*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :%sOther: %s - %s\n", me,
			    f->getrid().c_str(), date.c_str(),
			    typeid(*(f.get())).name(), f->str().c_str());
		} else if (dynamic_cast<EvError*>(e.get())) {
		    auto_ptr<EvError> f((EvError*)e.release());

		    fprintf(*c, ":%s NOTICE %s :%sError: %s\n", me,
			    nick.c_str(), date.c_str(), f->str().c_str());

		    if (dynamic_cast<EvNeedRelogin*>(f.get()))
			x->relogin();
		} else if (dynamic_cast<EvSysMsg*>(e.get())) {
		    auto_ptr<EvSysMsg> f((EvSysMsg*)e.release());

		    fprintf(*c, ":%s NOTICE %s :%sSystem: %s\n", me,
			    nick.c_str(), date.c_str(), f->str().c_str());
		} else if (dynamic_cast<EvNote*>(e.get())) {
		    auto_ptr<EvNote> f((EvNote*)e.release());
    
                    if (x->rooms.size()) {
			for (rooms_t::iterator i = x->rooms.begin();
			    i != x->rooms.end(); i++)
			    fprintf(*c, ":%s NOTICE #%s :%s\002Mas novy vzkaz!\002 "
				"[ %s ]\n",
				me, i->first.c_str(), date.c_str(),
				x->makepath("offline", PATH_STATIC).c_str());
		    } else {
			fprintf(*c, ":%s NOTICE %s :%s\002Mas novy vzkaz!\002 "
			    "[ %s ]\n",
			    me, nick.c_str(), date.c_str(),
			    x->makepath("offline", PATH_STATIC).c_str());
		    }
		} else if (dynamic_cast<EvSuperAdminsChange*>(e.get())) {
		    auto_ptr<EvSuperAdminsChange> f((EvSuperAdminsChange*)e.release());

                    if (x->rooms.size()) {
    			for (vector<string>::const_iterator i = f->getadded().begin();
			    i != f->getadded().end(); i++)
			    for (rooms_t::iterator j = x->rooms.begin();
				j != x->rooms.end(); j++)
				if (j->second.nicklist.find(*i) != j->second.nicklist.end())
				    fprintf(*c, ":%s MODE #%s +a %s\n", me,
					j->first.c_str(), i->c_str());

    			for (vector<string>::const_iterator i = f->getremoved().begin();
			    i != f->getremoved().end(); i++)
			    for (rooms_t::iterator j = x->rooms.begin();
				j != x->rooms.end(); j++)
				if (j->second.nicklist.find(*i) != j->second.nicklist.end())
				    fprintf(*c, ":%s MODE #%s -a %s\n", me,
					j->first.c_str(), i->c_str());
		    }
		} else {
		    fprintf(*c, ":%s NOTICE %s :%sOther: %s - %s\n", me,
			    nick.c_str(), date.c_str(),
			    typeid(*(e.get())).name(), e->str().c_str());
		}
	    }

	    /*
	     * Timeouts
	     */
	    if (!x.get()) {
		if (time(0) - connect_time > 60) {
		    fprintf(*c, "ERROR :Timeout\n");
		    break;
		}
	    } else {
		if (last_ping_sent && time(0) - last_ping_sent > 120) {
		    fprintf(*c, "ERROR :Timeout\n");
		    break;
		}

		if (time(0) - last_ping > 240) {
		    fprintf(*c, "PING :%s\n", me);
		    last_ping = last_ping_sent = time(0);
		}
	    }
	}
    } catch (exception &e) {
	cerr << e.what() << endl;
    }

    log(tomi_ntop(c->rname) + " - Disconnected (" + nick + ")");
}

int main(int argc, char *argv[])
{
    xchat_init();
#ifndef WIN32
    parent = getpid();
    signal(SIGCHLD, sigchld);
    signal(SIGHUP, sighup);
#endif

    int port = 6669;
    string addr = "::";
#ifdef WIN32
    bool detach = true;

    /*
     * Win32 does not have IPV6_V6ONLY until Vista, so we better have default
     * address as INADDR_ANY.
     */
    addr = "0.0.0.0";
#endif

    for (int arg = 1; arg < argc; arg++)
	if (argv[arg][0] == '-') {
	    if (argv[arg][1] == '-') {
		char *argp = argv[arg], *c = argp + 2, shortopt = ' ';

		if (strcmp(c, "help") == 0)
		    shortopt = 'h';
		else if (strcmp(c, "port") == 0)
		    shortopt = 'p';
		else if (strcmp(c, "bind") == 0)
		    shortopt = 'b';
		else if (strcmp(c, "log") == 0)
		    shortopt = 'l';
		else if (strcmp(c, "restrict") == 0)
		    shortopt = 'r';
		else if (strcmp(c, "conntimeout") == 0) {
		    arg++;
		    if (arg >= argc) {
			cerr << "--conntimeout needs parameter" << endl;
			return -1;
		    }
		    net::TomiHTTP::g_http_conn_timeout = atoi(argv[arg]);
		}
		else if (strcmp(c, "recvtimeout") == 0) {
		    arg++;
		    if (arg >= argc) {
			cerr << "--recvtimeout needs parameter" << endl;
			return -1;
		    }
		    net::TomiHTTP::g_http_recv_timeout = atoi(argv[arg]);
		}
#ifdef WIN32
		else if (strcmp(c, "foreground") == 0)
		    shortopt = 'f';
#endif
		else {
		    cerr << "Unknown option --" << c << ", use -h for help" << endl;
		    return -1;
		}

		if (shortopt == ' ')
		    continue;

		argp[1] = shortopt;
		for (c = argp + 2; *c; c++)
		    *c = ' ';
	    }

	    for (char *c = argv[arg] + 1; *c; c++)
		switch (*c) {
#ifdef WIN32
		    case 'f':
			detach = false;
			break;
#endif
		    case 'p':
			arg++;
			if (arg >= argc) {
			    cerr << "-p needs parameter" << endl;
			    return -1;
			}
			port = atoi(argv[arg]);
			break;
		    case 'b':
			arg++;
			if (arg >= argc) {
			    cerr << "-b needs parameter" << endl;
			    return -1;
			}
			addr = argv[arg];
			break;
		    case 'l':
			arg++;
			if (arg >= argc) {
			    cerr << "-l needs parameter" << endl;
			    return -1;
			}
			logfile = argv[arg];
			break;
		    case 'r':
			arg++;
			if (arg >= argc) {
			    cerr << "-r needs parameter" << endl;
			    return -1;
			}
			restrictfile = argv[arg];
			break;
		    case 'h':
			cout << "Usage: " << argv[0] << " [options]" << endl;
			cout << " --help/-h - this help" << endl;
			cout << " --port/-p <port> - listen on given port" << endl;
			cout << " --bind/-b <addr> - listen on given address" << endl;
			cout << " --log/-l <file> - log to given file" << endl;
			cout << " --restrict/-r <file> - restrict to nicks in given file" << endl;
			cout << " --conntimeout <secs> - set connect timeout" << endl;
			cout << " --recvtimeout <secs> - set receive timeout" << endl;
#ifdef WIN32
			cout << " --foreground/-f - run in console" << endl;
#endif
			return 0;
			break;
		    case ' ':
			// ignore.
			break;
		    default:
			cerr << "Unknown option -" << *c << ", use -h for help" << endl;
			return -1;
		}
	}
	else {
	    cerr << "Unknown option " << argv[arg] << ", use -h for help" << endl;
	    return -1;
	}

#ifdef WIN32
    if (detach)
	FreeConsole();
#endif

    compat_init_setproctitle(argc, argv);

    try {
	s.listen(port, addr);

	welcome();

	while (1) {
	    auto_ptr<TomiTCP> c(s.accept());

#ifdef WIN32
	    if (_beginthread((void(*)(void*))&serve_client, 0, c.release()) == -1)
		throw runtime_error(string(strerror(errno)));
#else
	    pid_t pid;
	    if ((pid = fork()) == 0) {
		s.close();
		serve_client(c.release());
		return 0;
	    } else if (pid == -1) {
		throw runtime_error(string(strerror(errno)));
	    }
#endif
	}
    } catch (exception &e) {
	cerr << e.what() << endl;
    }

    return 0;
}
