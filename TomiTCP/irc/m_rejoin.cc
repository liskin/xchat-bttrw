#include <iostream>
#include <unistd.h>
#include "irc.h"
using namespace std;

int rj_sleep = 5;

extern "C" {

int m_rejoin_config(string a, string b)
{
    if (!strcasecmp(a.c_str(),"rj_sleep"))
        rj_sleep = atol(b.c_str());
    else
        return 1;

    return 0;
}

void m_rejoin_msg(net::TomiTCP &f, string snick, string shost, vector<string> cmd)
{
    if (!strcasecmp(cmd[0].c_str(),"KICK") && !strcasecmp(cmd[2].c_str(),nick.c_str())) {
	sleep(rj_sleep);
	S(f,"JOIN %s\n",cmd[1].c_str());
    }

    if ((cmd[0] == "471" || cmd[0] == "473" || 
		cmd[0] == "474" || cmd[0] == "475")
	    && !strcasecmp(cmd[1].c_str(),nick.c_str())) {
	sleep(rj_sleep);
	S(f,"JOIN %s\n",cmd[2].c_str());
    }
}

void m_rejoin_init(struct module &m)
{
    cout << "m_rejoin init - hello" << endl;
    m.msg = m_rejoin_msg;
    m.config = m_rejoin_config;
}

}
