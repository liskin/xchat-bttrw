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

string striphtml(string a)
{
    string out;
    stringstream s(a);

    while (getline(s,a,'<')) {
	out += a + " ";
	getline(s,a,'>');
    }

    return out;
}

string hash(const string& s)
{
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

struct room {
    int l;
};
typedef map<string,room> rooms_t;
rooms_t rooms;

const char *me = "xchat.cz";
const char *userhost = "users.xchat.cz";

int main(int argc, char *argv[])
{
    try {
	s.listen(6669);
	c.reset(s.accept());

	string nick, pass;

	while (1) {
	    if (input_timeout(c->sock, 3000) > 0) {
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
		} else if (!x.get()) {
		    // registered command boundary
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

		    vector<string> nicklist;
		    nicklist.push_back(nick);

		    try { rooms[cmd[1]].l = x->join(cmd[1], nicklist); }
		    catch (runtime_error e) {
			fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			break;
		    }

		    fprintf(*c, ":%s!%s@%s JOIN #%s\n", nick.c_str(), hash(nick).c_str(),
			    userhost, cmd[1].c_str());
		    string tmp; int i; vector<string>::iterator j;
		    for (i = 1, j = nicklist.begin(); j != nicklist.end(); j++, i++) {
			tmp += *j + " ";
			if (i % 5 == 0 || j + 1 == nicklist.end()) {
			    fprintf(*c, ":%s 353 %s = #%s :%s\n", me, nick.c_str(),
				    cmd[1].c_str(), tmp.c_str());
			    tmp.clear();
			}
		    }
		    fprintf(*c, ":%s 366 %s #%s :End of /NAMES list.\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "PART" && cmd.size() >= 2) {
		    if (cmd[1][0] == '#')
			cmd[1].erase(cmd[1].begin());

		    if (rooms.find(cmd[1]) != rooms.end()) {
			x->part(cmd[1]);
			rooms.erase(cmd[1]);
			fprintf(*c, ":%s!%s@%s PART #%s :\n", nick.c_str(),
				hash(nick).c_str(), userhost, cmd[1].c_str());
		    } else {
			fprintf(*c, ":%s 403 %s %s :No such channel\n", me,
				nick.c_str(), cmd[1].c_str());
		    }
		} else if (cmd[0] == "PRIVMSG" && cmd.size() == 3) {
		    if (cmd[1][0] == '#') {
			cmd[1].erase(cmd[1].begin());

			try { x->putmsg(cmd[1], cmd[2]); }
			catch (runtime_error e) {
			    fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			    break;
			}
		    } else {
			if (rooms.size()) {
			    try { x->putmsg(rooms.begin()->first, "/s " + cmd[1] + " " + cmd[2]); }
			    catch (runtime_error e) {
				fprintf(*c, ":%s ERROR :%s\n", me, e.what());
				break;
			    }
			} else {
			    fprintf(*c, ":%s NOTICE :Can't send PRIVMSG's without channel joined\n", me);
			}
		    }
		} else if (cmd[0] == "MODE" && cmd.size() == 2) {
		    fprintf(*c, ":%s 324 %s %s +\n", me, nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "MODE" && cmd.size() == 3 && cmd[2][0] == 'b') {
		    fprintf(*c, ":%s 368 %s %s :End of Channel Ban List\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "WHO" && cmd.size() == 2) {
		    fprintf(*c, ":%s 315 %s %s :End of /WHO list.\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else {
		    cout << l << endl;
		    fprintf(*c, ":%s NOTICE %s :Unknown command\n", me, nick.c_str());
		}
	    }

	    if (x.get()) {
		try {
		    for (rooms_t::iterator j = rooms.begin(); j != rooms.end(); j++) {
			vector<string> m;
			j->second.l = x->getmsg(j->first, j->second.l, m);
			for (vector<string>::iterator i = m.begin(); i != m.end(); i++) {
			    string m = striphtml(*i);
			    XChat::stripdate(m);
			    string src = me, target = "#" + j->first;
			    XChat::getnick(m, src, target);
			    XChat::striphtmlent(m);

			    if (src != nick)
				fprintf(*c, ":%s!%s@%s PRIVMSG %s :%s\n", src.c_str(),
					hash(src).c_str(), userhost,
					target.c_str(), m.c_str());
			}
		    }
		}
		catch (runtime_error e) {
		    fprintf(*c, ":%s ERROR :%s\n", me, e.what());
		    break;
		}
	    }
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}
