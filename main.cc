#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <memory>
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

TomiTCP s;
auto_ptr<TomiTCP> c;
auto_ptr<XChat> x;

const char *me = "xchat.cz";

int main(int argc, char *argv[])
{
    /*if (argc != 4) {
	cerr << argv[0] << " <user> <pass> <channel>" << endl;
	return -1;
    }

    string room = argv[3];*/

    try {
	s.listen(6669);
	c.reset(s.accept());

	string nick, pass;

	string room; // remove ;)
	int room_l = 0;

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

		    fprintf(*c, ":%s 001 %s :Smazko!\n", me, nick.c_str());
		    try { x.reset(new XChat(nick, pass)); }
		    catch (runtime_error e) {
			fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			break;
		    }
		} else if (!x.get()) {
		    fprintf(*c, ":%s ERROR :Not registered\n", me);
		    break;
		}

		if (cmd[0] == "PING") {
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
		    room = cmd[1];

		    try { room_l = x->join(cmd[1]); }
		    catch (runtime_error e) {
			fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			break;
		    }

		    fprintf(*c, "%s!@ JOIN #%s\n", nick.c_str(), cmd[1].c_str());
		    fprintf(*c, ":%s 366 %s #%s :End of /NAMES list.\n", me,
			    nick.c_str(), cmd[1].c_str());
		} else if (cmd[0] == "PRIVMSG" && cmd.size() == 3) {
		    if (cmd[1][0] == '#')
			cmd[1].erase(cmd[1].begin());

		    try { x->putmsg(cmd[1], cmd[2]); }
		    catch (runtime_error e) {
			fprintf(*c, ":%s ERROR :%s\n", me, e.what());
			break;
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
		    if (room.length()) {
			vector<string> m;
			room_l = x->getmsg(room, room_l, m);
			for (vector<string>::iterator i = m.begin(); i != m.end(); i++) {
			    string m = striphtml(*i);
			    fprintf(*c, ":%s PRIVMSG #%s :%s\n", me, room.c_str(), m.c_str());
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
