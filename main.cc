#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <memory>
#include <map>
#include <queue>
#include <openssl/md5.h>
#include "xchat.h"
#include "irc.h"
#include "idle.h"
#include "TomiTCP/net.h"
#include "TomiTCP/str.h"
using namespace std;
using namespace xchat;
using namespace net;

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

rooms_t rooms;

const char *me = "xchat.cz";
const char *userhost = "users.xchat.cz";
const char *sexhost[] = {
    "girls.xchat.cz",
    "boys.xchat.cz"
};
const int mysex = 1;

x_nick* findnick(string nick)
{
    strtolower(nick);
    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
	nicklist_t::iterator n = i->second.nicklist.find(nick);
	if (n != i->second.nicklist.end())
	    return &n->second;
    }

    return 0;
}

const char * getsexhost(string src)
{
    strtolower(src);
    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++)
	if (i->second.nicklist.find(src) != i->second.nicklist.end())
	    return sexhost[i->second.nicklist[src].sex];

    return userhost;
}

time_t last_sent = 0, last_recv = 0;
int send_interval = 5, recv_interval = 3, idle_interval = 840;
queue<pair<string,string> > sendq;
inline void sendq_push(const string& a, const string& b) {
    sendq.push(pair<string,string>(a,b));
}

int main(int argc, char *argv[])
{
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

		    fprintf(*c, ":%s 001 %s :Vitej Smazko!\n", me, nick.c_str());
		    try { x.reset(new XChat(nick, pass)); }
		    catch (runtime_error e) {
			fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			break;
		    }
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
			rooms[cmd[1]] = x->join(cmd[1]);

			rooms[cmd[1]].nicklist[strtolower_nr(nick)] =
			    (struct x_nick){nick, mysex};

			fprintf(*c, ":%s!%s@%s JOIN #%s\n", nick.c_str(), hash(nick).c_str(),
				sexhost[mysex], cmd[1].c_str());
			string tmp; int i; nicklist_t::iterator j;
			for (i = 1, j = rooms[cmd[1]].nicklist.begin();
				j != rooms[cmd[1]].nicklist.end(); j++, i++) {
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

		    if (rooms.find(cmd[1]) != rooms.end()) {
			try {
			    x->part(cmd[1]);
			    rooms.erase(cmd[1]);
			    fprintf(*c, ":%s!%s@%s PART #%s :\n", nick.c_str(),
				    hash(nick).c_str(), sexhost[mysex], cmd[1].c_str());
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

			sendq_push(cmd[1], cmd[2]);
		    } else {
			if (rooms.size()) {
			    bool global = 1;
			    rooms_t::iterator r;
			    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++)
				if (i->second.nicklist.find(strtolower_nr(cmd[1]))
					!= i->second.nicklist.end()) {
				    global = 0;
				    r = i;
				    break;
				}

			    if (global)
				sendq_push(rooms.begin()->first, "/m " + cmd[1] + " " + cmd[2]);
			    else
				sendq_push(r->first, "/s " + cmd[1] + " " + cmd[2]);
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
			for (nicklist_t::iterator i = rooms[cmd[1]].nicklist.begin();
				i != rooms[cmd[1]].nicklist.end(); i++) {
			    fprintf(*c, ":%s 352 %s #%s %s %s %s %s %s :%d %s\n", me,
				    nick.c_str(), cmd[1].c_str(), hash(i->second.nick).c_str(),
				    sexhost[i->second.sex], me, i->second.nick.c_str(), "H", 0,
				    "xchat.cz user");
			}
			cmd[1] = "#" + cmd[1];
		    } else {
			x_nick *n = findnick(cmd[1]);
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

	    if (x.get() && !sendq.empty() && time(0) - last_sent >= send_interval) {
		pair<string,string> msg = sendq.front(); sendq.pop();
		try {
		    x->putmsg(msg.first, msg.second);
		} catch (runtime_error e) {
		    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
			    nick.c_str(), e.what());
		}
		last_sent = time(0);
		rooms[msg.first].last_sent = last_sent;
	    }

	    // f00king idler
	    if (sendq.empty()) {
		for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
		    if (time(0) - i->second.last_sent >= idle_interval) {
			sendq_push(i->first, "/s " + nick + " " +
				idle_msgs[rand() % idle_msgs_count]);
		    }
		}
	    }

	    if (time(0) - last_recv >= recv_interval && x.get()) {
		try {
		    for (rooms_t::iterator j = rooms.begin(); j != rooms.end(); j++) {
			vector<string> m;
			try {
			    j->second.l = x->getmsg(j->first, j->second.l, m);
			} catch (runtime_error e) {
			    try { x->part(j->first); } catch (...) { }
			    fprintf(*c, ":%s!%s@%s PART #%s :\n", nick.c_str(),
				    hash(nick).c_str(), sexhost[mysex], j->first.c_str());
			    fprintf(*c, ":%s NOTICE %s :Error: %s\n", me,
				    nick.c_str(), e.what());
			    rooms.erase(j->first);
			}
			for (vector<string>::iterator i = m.begin(); i != m.end(); i++) {
			    string m = XChat::striphtml(*i);
			    XChat::stripdate(m);
			    string src = me, target = "#" + j->first, reason, who;
			    XChat::getnick(m, src, target);
			    XChat::striphtmlent(m);

			    if (src == me && XChat::isjoin(m, rooms, src, j->first)) {
				fprintf(*c, ":%s!%s@%s JOIN #%s\n", src.c_str(),
					hash(src).c_str(), getsexhost(src),
					j->first.c_str());
			    } else if (src == me && XChat::ispart(m, rooms, src)) {
				fprintf(*c, ":%s!%s@%s PART #%s :No reason\n", src.c_str(),
					hash(src).c_str(), getsexhost(src),
					j->first.c_str());
				j->second.nicklist.erase(strtolower_nr(src));
			    } else if (src == me && XChat::iskick(m, rooms, src, reason, who)) {
				if (who.empty())
				    fprintf(*c, ":%s!%s@%s PART #%s :%s\n", src.c_str(),
					    hash(src).c_str(), getsexhost(src),
					    j->first.c_str(), reason.c_str());
				else
				    fprintf(*c, ":%s!%s@%s KICK #%s %s :%s\n", who.c_str(),
					    hash(who).c_str(), getsexhost(who),
					    j->first.c_str(), src.c_str(), reason.c_str());
				j->second.nicklist.erase(strtolower_nr(src));
			    } else if (strtolower_nr(src) == "system" && 
				    strtolower_nr(target) == strtolower_nr(nick)) {
				fprintf(*c, ":%s NOTICE %s :System: %s\n", me,
					nick.c_str(), m.c_str());
			    } else if (strtolower_nr(src) != strtolower_nr(nick))
				fprintf(*c, ":%s!%s@%s PRIVMSG %s :%s\n", src.c_str(),
					hash(src).c_str(), getsexhost(src),
					target.c_str(), m.c_str());
			}
		    }
		}
		catch (runtime_error e) {
		    fprintf(*c, ":%s ERROR :%s\n", me, e.what());
		    break;
		}
		last_recv = time(0);
	    }
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}
