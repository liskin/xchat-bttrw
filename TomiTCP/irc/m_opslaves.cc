#include <iostream>
#include <vector>
#include <string>
#include <fnmatch.h>
#include "irc.h"
using namespace std;

extern "C" {

void m_opslaves_msg(FILE *f, string snick, string shost, vector<string> cmd)
{
    if (!strcasecmp(cmd[0].c_str(),"JOIN")) {
	for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	    if ((snick + "!" + shost) == i->mask) {
		S(f,"MODE %s +o %s\n",cmd[1].c_str(),snick.c_str());
	    }
	}
    }
}

void m_opslaves_init(struct module &m)
{
    cout << "m_opslaves init - hello" << endl;
    m.msg = m_opslaves_msg;
}

}
