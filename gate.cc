#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <memory>
#include <map>
#include <openssl/md5.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "xchat.h"
#include "irc.h"
#include "idle.h"
#include "TomiTCP/net.h"
#include "TomiTCP/str.h"
using namespace std;
using namespace xchat;
using namespace net;

#define VERSION "SVN"

/*
 * Hash nick to make unique username
 */
string hash(string s)
{
    strtolower(s);
    unsigned char mdbuf[16]; char tmp[10];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, s.c_str(), s.length());
    MD5_Final(mdbuf, &md5);
    sprintf(tmp, "%.8x", *((unsigned int*)mdbuf));
    return tmp;
}

/*
 * Some global variables
 */
TomiTCP s;
auto_ptr<TomiTCP> c;
auto_ptr<XChat> x;

void sigchld(int) {
    wait(0);
}

int main(int argc, char *argv[])
{
    srand(time(0) ^ getpid());
    signal(SIGCHLD, sigchld);
    init_recode();

    int port = 6669;
    if (argc == 2 && atol(argv[1]))
	port = atol(argv[1]);

    try {
	s.listen(port);
main_accept:
	c.reset(s.accept());

	pid_t pid = fork();
	if (pid < 0)
	    return -1;
	if (pid > 0) {
	    c.reset(0);
	    goto main_accept;
	}

	string nick, pass;

	while (1) {
	    if (input_timeout(c->sock, 1000) > 0) {
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
		if (!x.get() && cmd[0] == "NICK" && cmd.size() >= 2) {
		    nick = cmd[1];
		} else if (!x.get() && cmd[0] == "PASS" && cmd.size() >= 2) {
		    pass = cmd[1];
		} else if (!x.get() && cmd[0] == "USER" && cmd.size() > 1) {
		    if (!nick.length() || !pass.length()) {
			fprintf(*c, ":%s ERROR :Need password and nick!\n", me);
			break;
		    }

		    try { x.reset(new XChat(nick, pass)); }
		    catch (runtime_error e) {
			fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			break;
		    }

		    /*
		     * Successful login, so output some numerics to make
		     * client happy.
		     */

		    fprintf(*c, ":%s 001 %s :Welcome Back To The Real World, but still connected to xchat.cz\n", me, nick.c_str());
		    fprintf(*c, ":%s 002 %s :Your host is %s[%s/%i]"
			    ", running version xchat-bttrw " VERSION "\n", me,
			    nick.c_str(), me, revers(c->lname).c_str(), port);
		    fprintf(*c, ":%s 003 %s :This server was created god knows when\n",
			    me, nick.c_str());
		    fprintf(*c, ":%s 004 %s :%s xchat-bttrw-" VERSION " 0 0\n",
			    me, nick.c_str(), me);
		    fprintf(*c, ":%s 005 %s :MODES=1 MAXTARGETS=1 NICKLEN=256\n", me, nick.c_str());
		    fprintf(*c, ":%s 005 %s :CHANTYPES=# PREFIX=() CHANMODES=,,,"
			    " NETWORK=xchat.cz CASEMAPPING=ascii\n", me, nick.c_str());
		} else if (!x.get()) {
		    /*
		     * If the user is not registered and sends some other
		     * command, quit him.
		     */
		    fprintf(*c, ":%s ERROR :Not registered\n", me);
		    break;

		    /*
		     * And here follow commands, which can be invoked after
		     * successful registration.
		     */
		} else if (cmd[0] == "SET" && cmd.size() >= 2) {
		    strtoupper(cmd[1]);
		    if (cmd[1] == "IDLE_INTERVAL" && cmd.size() == 3) {
			idle_interval = atol(cmd[2].c_str());
			fprintf(*c, ":%s NOTICE %s :idle_interval set to %i\n",
				me, nick.c_str(), idle_interval);
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
				    x->getsexhost(nick), chan.c_str());

			    // output userlist (NAMES)
			    string tmp; int i; nicklist_t::iterator j;
			    for (i = 1, j = x->rooms[chan].nicklist.begin();
				    j != x->rooms[chan].nicklist.end(); j++, i++) {
				tmp += j->second.nick + " ";
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
			    fprintf(*c, ":%s 403 %s #%s :%s\n", me, nick.c_str(),
				    chan.c_str(), e.what());
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
				x->part(chan);
				fprintf(*c, ":%s!%s@%s PART #%s :\n", nick.c_str(),
					hash(nick).c_str(), x->getsexhost(nick),
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
		} else if (cmd[0] == "PRIVMSG" && cmd.size() == 3) {
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			/*
			 * Channel message
			 */
			x->sendq_push(cmd[1], cmd[2]);
		    } else {
			/*
			 * Private message
			 */
			if (x->rooms.size()) {
			    // decide if we have to send global msg
			    room *r;
			    x_nick *n = x->findnick(cmd[1], &r);
			    if (n)
				x->sendq_push(r->rid, "/s " + cmd[1] + " " + cmd[2]);
			    else
				x->sendq_push(x->rooms.begin()->first, "/m " + cmd[1] +
					" " + cmd[2]);
			} else {
			    fprintf(*c, ":%s NOTICE %s :Can't send PRIVMSG's "
				    "without channel joined\n", me, nick.c_str());
			}
		    }
		} else if (cmd[0] == "MODE" && cmd.size() == 2) {
		    // just to make client's `channel synchronizing' happy
		    fprintf(*c, ":%s 324 %s %s +\n", me, nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "MODE" && cmd.size() == 3 && cmd[2][0] == 'b') {
		    // just to make client's `channel synchronizing' happy
		    fprintf(*c, ":%s 368 %s %s :End of Channel Ban List\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "WHO" && cmd.size() == 2) {
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			/*
			 * Output channel WHO
			 */
			for (nicklist_t::iterator i = x->rooms[cmd[1]].nicklist.begin();
				i != x->rooms[cmd[1]].nicklist.end(); i++) {
			    fprintf(*c, ":%s 352 %s #%s %s %s %s %s %s :%d %s\n", me,
				    nick.c_str(), cmd[1].c_str(), hash(i->second.nick).c_str(),
				    sexhost[i->second.sex], me, i->second.nick.c_str(), "H", 0,
				    "xchat.cz user");
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
		     */
		    if (x->rooms.size()) {
			x->sendq_push(x->rooms.begin()->first,
				string("/info") + ((cmd.size() != 2)?"2 ":" ")
				    + cmd[1]);
		    } else {
			fprintf(*c, ":%s NOTICE %s :Can't send PRIVMSG's "
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
		} else {
		    cout << l << endl;
		    fprintf(*c, ":%s NOTICE %s :Unknown command\n", me, nick.c_str());
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
		if (x->should_recv())
		    for (rooms_t::iterator j = x->rooms.begin(); j != x->rooms.end(); j++) {
			try { x->getmsg(j->second); }
			catch (runtime_error e) {
			    string rid = j->first;
			    try { x->part(j->first); } catch (...) { }
			    fprintf(*c, ":%s!%s@%s PART #%s :\n", nick.c_str(),
				    hash(nick).c_str(), x->getsexhost(nick), rid.c_str());
			    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				    nick.c_str(), e.what());
			}
		    }
	    }

	    /*
	     * Go through recv queue and process messages
	     */
	    while (x.get() && !x->recvq.empty()) {
		pair<string,string> i = x->recvq.front(); x->recvq.pop();
		string &m = i.second, src = me, target = "#" + i.first,
		       reason, who, host;

		XChat::striphtml(m);
		XChat::stripdate(m);
		XChat::getnick(m, src, target);
		striphtmlent(m);
		XChat::unsmilize(m);

		/*
		 * Now we should have it somewhat parsed, so try if it a
		 * system message, and display it properly.
		 */

		if (src == me && x->isjoin(i.first, m, src)) {
		    fprintf(*c, ":%s!%s@%s JOIN %s\n", src.c_str(),
			    hash(src).c_str(), x->getsexhost(src),
			    target.c_str());
		} else if (src == me && x->ispart(i.first, m, src, host)) {
		    fprintf(*c, ":%s!%s@%s PART %s :No reason\n", src.c_str(),
			    hash(src).c_str(), host.c_str(),
			    target.c_str());
		} else if (src == me && x->iskick(i.first, m, src, reason, who, host)) {
		    if (who.empty())
			fprintf(*c, ":%s!%s@%s PART %s :%s\n", src.c_str(),
				hash(src).c_str(), host.c_str(),
				target.c_str(), reason.c_str());
		    else
			fprintf(*c, ":%s!%s@%s KICK %s %s :%s\n", who.c_str(),
				hash(who).c_str(), host.c_str(),
				target.c_str(), src.c_str(), reason.c_str());
		} else if (strtolower_nr(src) == "system" && 
			strtolower_nr(target) == strtolower_nr(nick) &&
			checkidle(wstrip_nr(m))) {
		    fprintf(*c, ":%s NOTICE %s :System: %s [IDLER]\n", me,
			    nick.c_str(), m.c_str());
		} else if (strtolower_nr(src) == "system" && 
			strtolower_nr(target) == strtolower_nr(nick)) {
		    fprintf(*c, ":%s NOTICE %s :System: %s\n", me,
			    nick.c_str(), m.c_str());
		} else if (strtolower_nr(src) == "tip") {
		    // spam protection ;)
		} else if (strtolower_nr(src) != strtolower_nr(nick)) {
		    fprintf(*c, ":%s!%s@%s PRIVMSG %s :%s\n", src.c_str(),
			    hash(src).c_str(), x->getsexhost(src),
			    target.c_str(), m.c_str());
		}
	    }
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }

    exit_recode();

    return 0;
}
