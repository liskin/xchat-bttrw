#include <iostream>
#include <unistd.h>
#include "irc.h"
using namespace std;

extern "C" {

void m_rejoin_msg(FILE *f, string snick, string shost, vector<string> cmd)
{
    if (!strcasecmp(cmd[0].c_str(),"KICK") && !strcasecmp(cmd[2].c_str(),nick.c_str())) {
	sleep(5);
	S(f,"JOIN %s\n",cmd[1].c_str());
    }

    if ((cmd[0] == "471" || cmd[0] == "473" || 
		cmd[0] == "474" || cmd[0] == "475")
	    && !strcasecmp(cmd[1].c_str(),nick.c_str())) {
	sleep(5);
	S(f,"JOIN %s\n",cmd[2].c_str());
    }
}

void m_rejoin_init(struct module &m)
{
    cout << "m_rejoin init - hello" << endl;
    m.msg = m_rejoin_msg;
}

}
