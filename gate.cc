#undef LOG_ENABLED
#undef RESTRICT_GATEWAY

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
#ifdef RESTRICT_GATEWAY
# include <fnmatch.h>
#endif
#include <typeinfo>
#include "md5.h"
#include "xchat.h"
#include "irc.h"
#include "TomiTCP/net.h"
#include "TomiTCP/str.h"
#include "setproctitle.h"
using namespace std;
using namespace xchat;
using namespace net;

#define VERSION "SVN-" REVISION

/*
 * Hash nick to make unique username
 */
string hash(string s)
{
    strtolower(s);
    unsigned char mdbuf[16]; char tmp[10];
    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, (const unsigned char*)s.c_str(), s.length());
    MD5Final(mdbuf, &md5);
    sprintf(tmp, "%.8x", *((unsigned int*)mdbuf));
    return tmp;
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

#ifdef LOG_ENABLED
void log(const string& s)
{
    time_t t = time(0);
    struct tm lt;
    char st[128];

    localtime_r(&t, &lt);
    strftime(st, 128, "%F %H:%M:%S", &lt);

    FILE *f = fopen("gate-log", "a");
    if (f) {
	flockfile(f);
	funlockfile(f);
	fprintf(f, "%s [%i] - %s\n", st, getpid(), s.c_str());
	fclose(f);
    }
}
#endif

#ifdef RESTRICT_GATEWAY
bool check_restrict(const string& nick)
{
    ifstream f("gate-restrict");
    string l;

    while (getline(f, l)) {
	if (!fnmatch(l.c_str(), nick.c_str(), FNM_CASEFOLD))
	    return true;
    }

    return false;
}
#endif

/*
 * Some global variables
 */
TomiTCP s;
auto_ptr<TomiTCP> c;
auto_ptr<XChat> x;

const char * const me = "xchat.cz";
const char * const sexhost[] = {
    "girls.xchat.cz",
    "boys.xchat.cz",
    "users.xchat.cz"
};

time_t last_ping = 0, last_ping_sent = 0, connect_time = 0;

#ifndef WIN32
void sigchld(int) {
    int status, serrno;
    serrno = errno;
    while (1)
	if (waitpid (WAIT_ANY, &status, WNOHANG) <= 0)
	    break;
    errno = serrno;
}
#endif

void welcome()
{
    cout << "Hello, this is an IRC to xchat gateway from xchat-bttrw r." REVISION << endl;
    cout << "Built with TomiTCP r." TOMITCP_REV << endl;
    cout << "Coded by Tomas Janousek <tomi@nomi.cz> and others" << endl;
    cout << "Homepage: http://nomi.cz/projects.shtml?id=xchat-bttrw" << endl;
#ifdef WIN32
    cout << "This is a Win32 build. Should work on Windows Xp SP 1 and up." << endl;
#endif
    cout << endl;
    cout << "This is free software; see COPYRIGHT for copying conditions.  There is NO" << endl;
    cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
    cout << endl;
    cout << "Listening for incoming connections on " << tomi_ntop(s.lname) << "/" <<
	ntohs(PORT_SOCKADDR(s.lname)) << endl;
}

int main(int argc, char *argv[])
{
    xchat_init();
#ifndef WIN32
    signal(SIGCHLD, sigchld);
#endif

    int port = 6669;
    if (argc == 2 && atol(argv[1]))
	port = atol(argv[1]);


    compat_init_setproctitle(argc, argv);

    try {
	s.listen(port
#ifdef WIN32
		, "127.0.0.1"
#endif
		);

	welcome();

main_accept:
	c.reset(s.accept());
	connect_time = time(0);

#ifndef WIN32
	pid_t pid = fork();
	if (pid < 0)
	    return -1;
	if (pid > 0) {
	    c.reset(0);
	    goto main_accept;
	}
#endif

#ifdef LOG_ENABLED
	log(tomi_ntop(c->rname) + " - Connected");
#endif

	setproctitle(("gate: " + tomi_ntop(c->rname)).c_str());

	string nick, pass;
	bool user = false;

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

#ifdef RESTRICT_GATEWAY
			if (!check_restrict(nick)) {
			    fprintf(*c, ":%s ERROR :Unauthorized user!\n", me);
# ifdef LOG_ENABLED
			    log(tomi_ntop(c->rname) + " - Unauthorized - " + nick);
# endif
			    break;
			}
#endif

#ifdef LOG_ENABLED
			log(tomi_ntop(c->rname) + " - Logging in - " + nick);
#endif

			setproctitle(("gate: " + tomi_ntop(c->rname) + " (" + nick + ")").c_str());

			try { x.reset(new XChat(nick, pass)); }
			catch (runtime_error e) {
			    fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			    break;
			}

			/*
			 * Successful login, so output some numerics to make
			 * client happy.
			 */

			fprintf(*c, ":%s 001 %s :Welcome Back To The Real World,"
				" but still connected to xchat.cz\n", me, nick.c_str());
			fprintf(*c, ":%s 002 %s :Your host is %s[%s/%i]"
				", running version xchat-bttrw " VERSION "\n", me,
				nick.c_str(), me, revers(c->lname).c_str(), port);
			fprintf(*c, ":%s 003 %s :This server was created god knows when\n",
				me, nick.c_str());
			fprintf(*c, ":%s 004 %s :%s xchat-bttrw-" VERSION " 0 io\n",
				me, nick.c_str(), me);
			fprintf(*c, ":%s 005 %s :MODES=1 MAXTARGETS=1 NICKLEN=20 SAFELIST\n",
				me, nick.c_str());
			fprintf(*c, ":%s 005 %s :CHANTYPES=# PREFIX=(o)@ CHANMODES=,,,"
				" NETWORK=xchat.cz CASEMAPPING=ascii\n", me, nick.c_str());

			fprintf(*c, ":%s 375 %s :- %s Message of the Day -\n", me,
				nick.c_str(), me);

			ifstream copyright("COPYRIGHT");
			if (copyright) {
			    string motd;
			    while (getline(copyright, motd))
				fprintf(*c, ":%s 372 %s :%s\n", me, nick.c_str(),
					motd.c_str());
			}

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
		    if (cmd[1] == "IDLE_INTERVAL" && cmd.size() == 3) {
			idle_interval = atol(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :idle_interval set to %i\n",
				me, nick.c_str(), idle_interval);
		    } else if (cmd[1] == "CHARSET" && cmd.size() == 3) {
			client_charset = cmd[2];
			fprintf(*c, ":%s NOTICE %s :client_charset set to %s\n",
				me, nick.c_str(), client_charset.c_str());
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

			    fprintf(*c, ":%s!%s@%s JOIN #%s\n", nick.c_str(), hash(nick).c_str(),
				    sexhost[2], chan.c_str());
			    fprintf(*c, ":%s 332 %s #%s :%s\n", me, nick.c_str(), chan.c_str(),
				    (x->rooms[chan].name + " - " + x->rooms[chan].desc).c_str());

			    // output userlist (NAMES)
			    string tmp; int i; nicklist_t::iterator j;
			    for (i = 1, j = x->rooms[chan].nicklist.begin();
				    j != x->rooms[chan].nicklist.end(); j++, i++) {
				tmp += ((x->isadmin(chan, j->first))?"@":"") +
				    j->second.nick + " ";
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
			} catch (runtime_error e) {
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
				fprintf(*c, ":%s!%s@%s PART #%s :\n", nick.c_str(),
					hash(nick).c_str(), sexhost[2],
					chan.c_str());
			    } catch (runtime_error e) {
				fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
					nick.c_str(), e.what());
			    }
			} else {
			    fprintf(*c, ":%s 403 %s %s :No such channel\n", me,
				    nick.c_str(), chan.c_str());
			}
		    }
		} else if ((cmd[0] == "PRIVMSG" || cmd[0] == "NOTICE") && cmd.size() == 3) {
		    if (cmd[0] == "NOTICE")
			cmd[2] = "Notice: " + cmd[2];

		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			/*
			 * Channel message
			 */
			x->msg(cmd[1], cmd[2]);
		    } else {
			/*
			 * Private message
			 */
			try { x->whisper(cmd[1], cmd[2]); }
			catch (runtime_error e) {
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
		    bool admin = 1; /* x->isadmin(cmd[1], nick); */

		    for (vector<pair<string, string> >::iterator i = modes.begin();
			    i != modes.end(); i++) {
			if (i->first == "?b") {
			    // just to make client's `channel synchronizing' happy
			    fprintf(*c, ":%s 368 %s #%s :End of Channel Ban List\n", me,
				    nick.c_str(), cmd[1].c_str());
			} else if (i->first == "+o") {
			    if (admin) {
				x->admin(cmd[1], i->second);
				admin = 0;
			    }
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
				fprintf(*c, ":%s 352 %s #%s %s %s %s %s %s :%d %s\n", me,
					nick.c_str(), cmd[1].c_str(), hash(i->second.nick).c_str(),
					sexhost[i->second.sex], me, i->second.nick.c_str(), 
					(x->isadmin(cmd[1], i->first))?"H@":"H", 0,
					"xchat.cz user");
			    }
			}
			cmd[1] = "#" + cmd[1];
		    } else {
			/*
			 * Output user WHO
			 */
			x_nick *n = x->findnick(cmd[1], 0);
			if (n)
			    fprintf(*c, ":%s 352 %s %s %s %s %s %s %s :%d %s\n", me,
				    nick.c_str(), "*", hash(n->nick).c_str(),
				    sexhost[n->sex], me, n->nick.c_str(), "H", 0,
				    "xchat.cz user");
		    }
		    fprintf(*c, ":%s 315 %s %s :End of /WHO list.\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "WHOIS" && cmd.size() >= 2) {
		    /*
		     * Mangle `WHOIS nick' into `/info nick'
		     * or
		     * Mangle `WHOIS nick nick' into `/info2 nick'
		     * Also, output a simple WHOIS reply.
		     */
		    if (x->rooms.size()) {
			x->msg(x->rooms.begin()->first,
				string("/info") + ((cmd.size() != 2)?"2 ":" ")
				    + cmd[1]);
		    } else {
			fprintf(*c, ":%s NOTICE %s :Can't do WHOIS "
				"without channel joined\n", me, nick.c_str());
		    }

		    x_nick *n = x->findnick(cmd[1], 0);
		    if (n)
			fprintf(*c, ":%s 311 %s %s %s %s * :%s\n", me,
				nick.c_str(), n->nick.c_str(),
				hash(n->nick).c_str(), sexhost[n->sex],
				"xchat.cz user");
		    fprintf(*c, ":%s 318 %s %s :End of /WHOIS list.\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "KICK" && cmd.size() >= 3) {
		    /*
		     * Kick user.
		     */
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());
		    }

		    x->kick(cmd[1], cmd[2], (cmd.size() > 3)?cmd[3]:"");
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
				(x->rooms[cmd[1]].name + " - " + x->rooms[cmd[1]].desc).c_str());
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
			    tmp += ((x->isadmin(cmd[1], j->first))?"@":"") +
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
			x->list(listout);
			for (listout_t::iterator i = listout.begin();
				i != listout.end(); i++) {
			    fprintf(*c, ":%s 322 %s #%s %i :%s\n", me, nick.c_str(),
				    i->first.c_str(), 0, i->second.c_str());
			}
			fprintf(*c, ":%s 323 %s :End of /LIST\n", me, nick.c_str());
		    } catch (runtime_error e) {
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
				hash(n->nick) + "@" + sexhost[n->sex] + " ";
		    }
		    fprintf(*c, ":%s 302 %s :%s\n", me, nick.c_str(), rpl.c_str());
		} else if (cmd[0] == "ISON" && cmd.size() >= 2) {
		    /*
		     * Output an ISON reply
		     */
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
		} else {
		    cout << l << endl;
		    fprintf(*c, ":%s 421 %s %s :Unknown command\n", me, nick.c_str(),
			    cmd[0].c_str());
		}
	    }

	    if (x.get()) {
		/*
		 * Let x do it's job on send queue
		 */
		try { x->do_sendq(); }
		catch (runtime_error e) {
		    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
			    nick.c_str(), e.what());
		}

		/*
		 * Receive some data sometimes
		 */
		try { x->fill_recvq(); }
		catch (runtime_error e) {
		    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
			    nick.c_str(), e.what());
		}
	    }

	    /*
	     * Go through recv queue and process messages
	     */
	    while (x.get() && !x->recvq.empty()) {
		auto_ptr<Event> e(x->recvq_pop());

		if (dynamic_cast<EvRoomError*>(e.get())) {
		    auto_ptr<EvRoomError> f((EvRoomError*)e.release());

		    if (f->isfatal()) {
			try { x->leave(f->getrid()); } catch (...) { }
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

		    fprintf(*c, ":%s!%s@%s %s #%s :%s\n", f->getsrc().nick.c_str(),
			    hash(f->getsrc().nick).c_str(), sexhost[f->getsrc().sex],
			    notice?"NOTICE":"PRIVMSG", f->getrid().c_str(),
			    str.c_str());
		} else if (dynamic_cast<EvWhisper*>(e.get())) {
		    auto_ptr<EvWhisper> f((EvWhisper*)e.release());
		    string str = f->str();
		    bool notice = is_notice(str);

		    fprintf(*c, ":%s!%s@%s %s %s :%s\n", f->getsrc().nick.c_str(),
			    hash(f->getsrc().nick).c_str(), sexhost[f->getsrc().sex],
			    notice?"NOTICE":"PRIVMSG", f->gettarget().c_str(),
			    str.c_str());
		} else if (dynamic_cast<EvRoomJoin*>(e.get())) {
		    auto_ptr<EvRoomJoin> f((EvRoomJoin*)e.release());

		    fprintf(*c, ":%s!%s@%s JOIN #%s\n", f->getsrc().nick.c_str(),
			    hash(f->getsrc().nick).c_str(), sexhost[f->getsrc().sex],
			    f->getrid().c_str());
		    if (x->ispermadmin(f->getrid(), f->getsrc().nick))
			fprintf(*c, ":%s MODE #%s +o %s\n", me,
				f->getrid().c_str(), f->getsrc().nick.c_str());
		} else if (dynamic_cast<EvRoomLeave*>(e.get())) {
		    auto_ptr<EvRoomLeave> f((EvRoomLeave*)e.release());

		    fprintf(*c, ":%s!%s@%s PART #%s :%s\n", f->getsrc().nick.c_str(),
			    hash(f->getsrc().nick).c_str(), sexhost[f->getsrc().sex],
			    f->getrid().c_str(), f->getreason().c_str());
		} else if (dynamic_cast<EvRoomKick*>(e.get())) {
		    auto_ptr<EvRoomKick> f((EvRoomKick*)e.release());

		    fprintf(*c, ":%s!%s@%s KICK #%s %s :%s\n", f->getsrc().nick.c_str(),
			    hash(f->getsrc().nick).c_str(), sexhost[f->getsrc().sex],
			    f->getrid().c_str(), f->gettarget().nick.c_str(),
			    f->getreason().c_str());
		} else if (dynamic_cast<EvRoomSysMsg*>(e.get())) {
		    auto_ptr<EvRoomSysMsg> f((EvRoomSysMsg*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :System: %s\n", me,
			    f->getrid().c_str(), f->str().c_str());
		} else if (dynamic_cast<EvRoomIdlerMsg*>(e.get())) {
		    auto_ptr<EvRoomIdlerMsg> f((EvRoomIdlerMsg*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :System: %s [IDLER]\n", me,
			    f->getrid().c_str(), f->str().c_str());
		} else if (dynamic_cast<EvRoomSysText*>(e.get())) {
		    auto_ptr<EvRoomSysText> f((EvRoomSysText*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :%s\n", me,
			    f->getrid().c_str(), f->str().c_str());
		} else if (dynamic_cast<EvRoomAdminChange*>(e.get())) {
		    auto_ptr<EvRoomAdminChange> f((EvRoomAdminChange*)e.release());

		    if (f->getbefore().empty() ||
			    x->ispermadmin(f->getrid(), f->getbefore()))
			fprintf(*c, ":%s MODE #%s +o %s\n", me,
				f->getrid().c_str(), f->getnow().c_str());
		    else if (f->getnow().empty())
			fprintf(*c, ":%s MODE #%s -o %s\n", me,
				f->getrid().c_str(), f->getbefore().c_str());
		    else
			fprintf(*c, ":%s MODE #%s -o+o %s %s\n", me,
				f->getrid().c_str(), f->getbefore().c_str(),
				f->getnow().c_str());
		} else if (dynamic_cast<EvRoomLockChange*>(e.get())) {
		    auto_ptr<EvRoomLockChange> f((EvRoomLockChange*)e.release());

		    fprintf(*c, ":%s MODE #%s %si\n", me,
			    f->getrid().c_str(), (f->getnow())?"+":"-");
		} else if (dynamic_cast<EvRoomAdvert*>(e.get())) {
		    auto_ptr<EvRoomAdvert> f((EvRoomAdvert*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :Advert: %s [ %s ]\n", me,
			    f->getrid().c_str(), f->str().c_str(),
			    f->getlink().c_str());
		} else if (dynamic_cast<EvRoomOther*>(e.get())) {
		    auto_ptr<EvRoomOther> f((EvRoomOther*)e.release());

		    fprintf(*c, ":%s NOTICE #%s :Other: %s - %s\n", me,
			    f->getrid().c_str(), typeid(*(f.get())).name(),
			    f->str().c_str());
		} else if (dynamic_cast<EvError*>(e.get())) {
		    auto_ptr<EvError> f((EvError*)e.release());

		    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
			    nick.c_str(), f->str().c_str());
		} else if (dynamic_cast<EvSysMsg*>(e.get())) {
		    auto_ptr<EvSysMsg> f((EvSysMsg*)e.release());

		    fprintf(*c, ":%s NOTICE %s :System: %s\n", me,
			    nick.c_str(), f->str().c_str());
		} else {
		    fprintf(*c, ":%s NOTICE %s :Other: %s - %s\n", me,
			    nick.c_str(), typeid(*(e.get())).name(),
			    e->str().c_str());
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

#ifdef LOG_ENABLED
	log(tomi_ntop(c->rname) + " - Disconnected (" + nick + ")");
#endif

#ifdef WIN32
	goto main_accept;
#endif
    } catch (runtime_error e) {
	cerr << e.what() << endl;
#ifdef LOG_ENABLED
	log(tomi_ntop(c->rname) + " - Disconnected - " + e.what());
#endif
    }

    return 0;
}
