#include <iostream>
#include <string>
#include "irc.h"
#include "smiles.h"
using namespace std;

void unsmilize(string &s)
{
    string::size_type a, b, pos = 0;

    while (((a = s.find('*', pos)) != string::npos) &&
	    ((b = s.find('*', a + 1)) != string::npos)) {
	bool fail = 0;
	for (string::iterator i = s.begin() + a + 1; i != s.begin() + b; i++)
	    fail |= !isdigit(*i);

	if (!fail) {
	    int smile = atol(string(s.begin() + a + 1, s.begin() + b).c_str());
	    if (smile < smiles_count && smiles[smile]) {
		s.erase(s.begin() + a, s.begin() + b + 1);
		s.insert(a, string("\002") + smiles[smile] + "\002");

		int add = 2;
		if (s[a + strlen(smiles[smile]) + 2] != ' ') {
		    s.insert(a + strlen(smiles[smile]) + 2, " ");
		    add++;
		}
		if (a > 0 && s[a - 1] != ' ') {
		    s.insert(a, " ");
		    add++;
		}

		pos = a + strlen(smiles[smile]) + add;
	    } else {
		pos = a + 1;
	    }
	} else {
	    pos = a + 1;
	}
    }
}

extern "C" {

void m_xchatsux_msg(string snick, string shost, vector<string> cmd)
{
    if (!strcasecmp(cmd[0].c_str(),"PRIVMSG") && cmd.size() >= 3 &&
	    cmd[1][0] == '#') {
	string ntext = cmd[2];
	unsmilize(ntext);

	if (ntext != cmd[2]) {
	    S(f, "PRIVMSG %s :<%s> %s\n", cmd[1].c_str(), snick.c_str(), ntext.c_str());
	}
    }
}

void m_xchatsux_init(struct module &m)
{
    cout << "m_xchatsux init - hello" << endl;
    m.msg = m_xchatsux_msg;
}

}
