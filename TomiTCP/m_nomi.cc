#include <iostream>
#include <vector>
#include <string>
#include <fnmatch.h>
#include "irc.h"
using namespace std;

vector<string> nomi_channels;
vector<string> nomi_ops;
string nomi_nick;

extern "C" {

int m_nomi_config(string a, string b)
{
    if (!strcasecmp(a.c_str(),"nomi_channels"))
	nomi_channels.push_back(b);
    else if (!strcasecmp(a.c_str(),"nomi_ops"))
	nomi_ops.push_back(b);
    else if (!strcasecmp(a.c_str(),"nomi_nick"))
	nomi_nick = b;
    else
	return 1;

    return 0;
}

void m_nomi_connected(FILE *f)
{
    if (nick != nomi_nick) {
	nick = nomi_nick;
	S(f,"KILL %s :Uhni svino!\nNICK %s\n",nick.c_str(),nick.c_str());
    }

    for (vector<string>::iterator i = nomi_channels.begin();
	    i != nomi_channels.end(); i++) {
	S(f,"OJOIN @%s\n",i->c_str());
    }
}

void m_nomi_mode(FILE *f, string snick, string shost, string chan,
	vector<string> modes)
{
    for (vector<string>::iterator i = modes.begin(); i != modes.end(); i++) {
	if (!strcasecmp(i->c_str(),("-o "+nick).c_str())) {
	    S(f,"KILL %s :Do kouta blbecku! Nedeopej rul3ra!\n",snick.c_str());
	    S(f,"PART %s :Du si pro opa!\n",chan.c_str());
	}
    }
}

void m_nomi_msg(FILE *f, string snick, string shost, vector<string> cmd)
{
    if (!strcasecmp(cmd[0].c_str(),"KICK") && !strcasecmp(cmd[2].c_str(),nick.c_str())) {
	    S(f,"KILL %s :Do kouta blbecku! Nekickej rul3ra!\n",snick.c_str());
	    S(f,"OJOIN @%s\n",cmd[1].c_str());
    }

    if (!strcasecmp(cmd[0].c_str(),"PART") &&
	    !strcasecmp(snick.c_str(),nick.c_str())) {
	S(f,"OJOIN @%s\n",cmd[1].c_str());
    }

    if (!strcasecmp(cmd[0].c_str(),"NOTICE") &&
	    !strcasecmp(cmd[1].c_str(),nick.c_str())) {
	if (!strncasecmp(cmd[2].c_str(),"op ",3)) {
	    int d = string(cmd[2],3).find(' ');
	    if (d != -1) {
		string chan = string(cmd[2],3,d);
		string pass = string(cmd[2],3+d+1);
		string t = snick+"!"+shost+" "+chan+" "+pass;

		for (vector<string>::iterator i = nomi_ops.begin();
			i != nomi_ops.end(); i++) {
		    if (!fnmatch(i->c_str(),t.c_str(),FNM_CASEFOLD)) {
			S(f,"MODE %s +o %s\n",chan.c_str(),snick.c_str());
		    }
		}
	    }
	}
    }

    if (cmd[0] == "403" && !strcasecmp(cmd[1].c_str(),nick.c_str())) {
	S(f,"JOIN %s\n",cmd[2].c_str());
    }
}

void m_nomi_init(struct module &m)
{
    cout << "m_nomi init - hello" << endl;
    m.config = m_nomi_config;
    m.connected = m_nomi_connected;
    m.mode = m_nomi_mode;
    m.msg = m_nomi_msg;
}

}
