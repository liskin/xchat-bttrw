#include <iostream>
#include <vector>
#include <string>
#include <fnmatch.h>
#include "irc.h"
using namespace std;

vector<string> ch_channels;

extern "C" {

int m_channel_config(string a, string b)
{
    if (!strcasecmp(a.c_str(),"ch_channels"))
	ch_channels.push_back(b);
    else
	return 1;

    return 0;
}

void m_channel_connected(FILE *f)
{
    for (vector<string>::iterator i = ch_channels.begin();
	    i != ch_channels.end(); i++) {
	S(f,"JOIN %s\n",i->c_str());
    }
}

void m_channel_init(struct module &m)
{
    cout << "m_channel init - hello" << endl;
    m.config = m_channel_config;
    m.connected = m_channel_connected;
}

}
