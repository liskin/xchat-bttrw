#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <memory>
#include <map>
#include <openssl/md5.h>
#include "xchat.h"
#include "irc.h"
#include "TomiTCP/net.h"
#include "TomiTCP/str.h"
using namespace std;
using namespace xchat;
using namespace net;

#define VERSION "SVN"

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

TomiTCP s;
auto_ptr<TomiTCP> c;
auto_ptr<XChat> x;

int main(int argc, char *argv[])
{
    srand(time(0) ^ getpid());

    int port = 6669;
    if (argc == 2 && atol(argv[1]))
	port = atol(argv[1]);

    try {
	s.listen(port);
	c.reset(s.accept());

	string nick, pass;

	while (1) {
	    if (input_timeout(c->sock, 1000) > 0) {
		string l, prefix;
		vector<string> cmd;
		c->getline(l); chomp(l); if (!l.length()) break;

		parsein(l, prefix, cmd);
		if (!cmd.size()) continue;

		strtoupper(cmd[0]);

		if (cmd[0] == "NICK" && cmd.size() >= 2) {
		    nick = cmd[1];
		} else if (cmd[0] == "PASS" && cmd.size() >= 2) {
		    pass = cmd[1];
		} else if (cmd[0] == "USER" && cmd.size() > 1) {
		    if (!nick.length() || !pass.length()) {
			fprintf(*c, ":%s ERROR :Need password and nick!\n", me);
			break;
		    }

		    try { x.reset(new XChat(nick, pass)); }
		    catch (runtime_error e) {
			fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			break;
		    }

		    fprintf(*c, ":%s 001 %s :Vitej Smazko na xchat.cz!\n", me, nick.c_str());
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
		} else if (!x.get()) { // -- registered command boundary --
		    fprintf(*c, ":%s ERROR :Not registered\n", me);
		    break;
		} else if (cmd[0] == "PING") {
		    if (cmd.size() >= 2) {
			fprintf(*c, ":%s PONG :%s\n", me, cmd[1].c_str());
		    } else {
			fprintf(*c, ":%s PONG %s\n", me, me);
		    }
		} else if (cmd[0] == "QUIT") {
		    break;
		} else if (cmd[0] == "JOIN" && cmd.size() >= 2) {
		    if (cmd[1][0] == '#')
			cmd[1].erase(cmd[1].begin());

		    try {
			x->join(cmd[1]);

			fprintf(*c, ":%s!%s@%s JOIN #%s\n", nick.c_str(), hash(nick).c_str(),
				x->getsexhost(nick), cmd[1].c_str());
			string tmp; int i; nicklist_t::iterator j;
			for (i = 1, j = x->rooms[cmd[1]].nicklist.begin();
				j != x->rooms[cmd[1]].nicklist.end(); j++, i++) {
			    tmp += j->second.nick + " ";
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
		    } catch (runtime_error e) {
			fprintf(*c, ":%s 403 %s #%s :%s\n", me, nick.c_str(),
				cmd[1].c_str(), e.what());
		    }
		} else if (cmd[0] == "PART" && cmd.size() >= 2) {
		    if (cmd[1][0] == '#')
			cmd[1].erase(cmd[1].begin());

		    if (x->rooms.find(cmd[1]) != x->rooms.end()) {
			try {
			    x->part(cmd[1]);
			    fprintf(*c, ":%s!%s@%s PART #%s :\n", nick.c_str(),
				    hash(nick).c_str(), x->getsexhost(nick), cmd[1].c_str());
			} catch (runtime_error e) {
			    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				    nick.c_str(), e.what());
			}
		    } else {
			fprintf(*c, ":%s 403 %s %s :No such channel\n", me,
				nick.c_str(), cmd[1].c_str());
		    }
		} else if (cmd[0] == "PRIVMSG" && cmd.size() == 3) {
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			x->sendq_push(cmd[1], cmd[2]);
		    } else {
			if (x->rooms.size()) {
			    bool global = 1;
			    rooms_t::iterator r;
			    for (rooms_t::iterator i = x->rooms.begin();
				    i != x->rooms.end(); i++)
				if (i->second.nicklist.find(strtolower_nr(cmd[1]))
					!= i->second.nicklist.end()) {
				    global = 0;
				    r = i;
				    break;
				}

			    if (global)
				x->sendq_push(x->rooms.begin()->first, "/m " + cmd[1] + " " + cmd[2]);
			    else
				x->sendq_push(r->first, "/s " + cmd[1] + " " + cmd[2]);
			} else {
			    fprintf(*c, ":%s NOTICE %s :Can't send PRIVMSG's "
				    "without channel joined\n", me, nick.c_str());
			}
		    }
		} else if (cmd[0] == "MODE" && cmd.size() == 2) {
		    fprintf(*c, ":%s 324 %s %s +\n", me, nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "MODE" && cmd.size() == 3 && cmd[2][0] == 'b') {
		    fprintf(*c, ":%s 368 %s %s :End of Channel Ban List\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "WHO" && cmd.size() == 2) {
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());
			for (nicklist_t::iterator i = x->rooms[cmd[1]].nicklist.begin();
				i != x->rooms[cmd[1]].nicklist.end(); i++) {
			    fprintf(*c, ":%s 352 %s #%s %s %s %s %s %s :%d %s\n", me,
				    nick.c_str(), cmd[1].c_str(), hash(i->second.nick).c_str(),
				    sexhost[i->second.sex], me, i->second.nick.c_str(), "H", 0,
				    "xchat.cz user");
			}
			cmd[1] = "#" + cmd[1];
		    } else {
			x_nick *n = x->findnick(cmd[1]);
			if (n)
			    fprintf(*c, ":%s 352 %s %s %s %s %s %s %s :%d %s\n", me,
				    nick.c_str(), "*", hash(n->nick).c_str(),
				    sexhost[n->sex], me, n->nick.c_str(), "H", 0,
				    "xchat.cz user");
		    }
		    fprintf(*c, ":%s 315 %s %s :End of /WHO list.\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else {
		    cout << l << endl;
		    fprintf(*c, ":%s NOTICE %s :Unknown command\n", me, nick.c_str());
		}
	    }

	    if (x.get()) {
		try { x->do_sendq(); }
		catch (runtime_error e) {
		    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
			    nick.c_str(), e.what());
		}

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

	    while (x.get() && !x->recvq.empty()) {
		pair<string,string> i = x->recvq.front(); x->recvq.pop();
		string &m = i.second, src = me, target = "#" + i.first,
		       reason, who, host;

		XChat::striphtml(m);
		XChat::stripdate(m);
		XChat::getnick(m, src, target);
		XChat::striphtmlent(m);
		XChat::unsmilize(m);

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
			strtolower_nr(target) == strtolower_nr(nick)) {
		    fprintf(*c, ":%s NOTICE %s :System: %s\n", me,
			    nick.c_str(), m.c_str());
		} else if (strtolower_nr(src) != strtolower_nr(nick)) {
		    fprintf(*c, ":%s!%s@%s PRIVMSG %s :%s\n", src.c_str(),
			    hash(src).c_str(), x->getsexhost(src),
			    target.c_str(), m.c_str());
		}
	    }
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}
