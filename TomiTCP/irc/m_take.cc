#include <iostream>
#include <unistd.h>
#include <fnmatch.h>
#include "irc.h"
using namespace std;

extern "C" {

string autotake;

void takeover(FILE *f, string chan)
{
    if (!channels[chan][nick]) {
	cout << "Not opped or not in" << endl;
	return;
    }

    // this need not to be here... :)
    if (safe_mode)
	processsome(f);

    vector<string> ops;

    for (channel_t::iterator i = channels[chan].begin(); i != channels[chan].end(); i++) {
	if (!i->second)
	    continue;

	if (!strcasecmp(i->first.c_str(),nick.c_str()))
	    continue;

	{
	    bool nn = 0;
	    for (vector<string>::iterator it = masters.begin(); it != masters.end(); it++)
		if (!fnmatch(it->c_str(),users[i->first].c_str(),FNM_CASEFOLD)) {
		    nn = 1;
		    break;
		}
	    if (nn)
		continue;
	}

	ops.push_back(i->first);
    }

    string out;
    for (unsigned int i=0; i<ops.size(); i+=max_modes) {
	string c = "MODE "+chan+" -";
	for (unsigned int j=i; j<(ops.size() <? (i+max_modes)); j++)
	    c += "o";
	for (unsigned int j=i; j<(ops.size() <? (i+max_modes)); j++)
	    c += " "+ops[j];
	if (safe_mode > 1) {
	    S(f,"%s\n",c.c_str());
	    if (safe_mode)
		processsome(f);
	} else {
	    out += c + "\n";
	}
    }
    if (safe_mode < 2) {
	if (out.length())
	    S(f,"%s",out.c_str());
	if (safe_mode)
	    processsome(f);
    }
}

void m_take_cmd(FILE *f, string snick, vector<string> cl)
{
    if (cl[0] == ".autotake") {
	if (cl.size() != 2) {
	    S(f,"PRIVMSG %s :Need 1 parameters\n",snick.c_str());
	} else {
	    autotake = cl[1].c_str();
	    S(f,"PRIVMSG %s :autotake = %s\n",snick.c_str(),autotake.c_str());
	}
	return;
    }

    if (cl[0] == ".take") {
	if (cl.size() != 2) {
	    S(f,"PRIVMSG %s :Need 1 parameters\n",snick.c_str());
	} else {
	    takeover(f,cl[1]);
	}
	return;
    }
}

void m_take_mode(FILE *f, string snick, string shost, string chan, vector<string> modes)
{
    if (strcasestr(autotake.c_str(),chan.c_str())) {
	for (vector<string>::iterator it = modes.begin(); it != modes.end(); it++) {
	    if (!strcasecmp(it->c_str(),(string("+o ")+nick).c_str())) {
		takeover(f,chan);
		break;
	    }
	}
    }
}

void m_take_init(struct module &m)
{
    cout << "m_take init - hello" << endl;
    m.cmd = m_take_cmd;
    m.mode = m_take_mode;
}

}
