#include <iostream>
#include <unistd.h>
#include <fnmatch.h>
#include "irc.h"
#include "../str.h"
using namespace std;

extern "C" {

string autotake;
vector<string> take_waiting;

void takeover(net::TomiTCP &f, string chan)
{
    {
	string tnick = nick;
	strtolower(tnick);
	if (!channels[chan][tnick]) {
	    cout << "Not opped or not in" << endl;
	    return;
	}
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
		if (!fnmatch(it->c_str(), (i->first + "!" + users[i->first]).c_str(),
			    FNM_CASEFOLD)) {
		    nn = 1;
		    break;
		}
	    if (!nn)
		for (slaves_t::iterator j = slaves.begin(); j != slaves.end(); j++) {
		    if (j->mask == (i->first + "!" + users[i->first])) {
			nn = 1;
			break;
		    }
		}
	    if (nn)
		continue;
	}

	ops.push_back(i->first);
    }

    unsigned int slave = 0;
    vector<string> sls(slaves.size());

    string out;
    for (unsigned int i=0; i<ops.size(); i+=max_modes) {
	string c = "MODE "+chan+" -";
	for (unsigned int j=i; j<(ops.size() <? (i+max_modes)); j++)
	    c += "o";
	for (unsigned int j=i; j<(ops.size() <? (i+max_modes)); j++)
	    c += " "+ops[j];
	if (slave == 0) {
	    if (safe_mode > 1) {
		S(f,"%s\n",c.c_str());
		if (safe_mode)
		    processsome(f);
	    } else {
		out += c + "\n";
	    }
	} else {
	    sls[slave - 1] += c + "\n";
	}

	slave++;
	if (slave > slaves.size()) {
	    slave = 0;
	}
    }

    slave = 0;
    for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	fprintf(*(i->s),"%s",sls[slave++].c_str());
    }

    if (safe_mode < 2) {
	if (out.length())
	    S(f,"%s",out.c_str());
	if (safe_mode)
	    processsome(f);
    }
}

void m_take_cmd(net::TomiTCP &f, string snick, vector<string> cl)
{
    if (cl[0] == ".autotake") {
	if (cl.size() != 2) {
	    S(f,"PRIVMSG %s :Need 1 parameters\n",snick.c_str());
	} else {
	    autotake = strtolower(cl[1]);
	    S(f,"PRIVMSG %s :autotake = %s\n",snick.c_str(),autotake.c_str());
	}
	return;
    }

    if (cl[0] == ".take") {
	if (cl.size() != 2) {
	    S(f,"PRIVMSG %s :Need 1 parameters\n",snick.c_str());
	} else {
	    strtolower(cl[1]);
	    for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
		string slnick,slhost;
		splitprefix(i->mask,slnick,slhost);

		if (!channels[cl[1]][slnick]) {
		    fprintf(*(i->s),"JOIN %s\n",cl[1].c_str());
		}
	    }
	    if (!slaves.size())
		takeover(f,cl[1]);
	    else
		take_waiting.push_back(cl[1]);
	}
	return;
    }
}

void m_take_mode(net::TomiTCP &f, string snick, string shost, string chan, vector<string> modes)
{
    strtolower(chan);
    bool me_opped = 0;
    if (strcasestr(autotake.c_str(),chan.c_str())) {
	for (vector<string>::iterator it = modes.begin(); it != modes.end(); it++) {
	    if (!strcasecmp(it->c_str(),(string("+o ")+nick).c_str())) {
		me_opped = 1;
		break;
	    }
	}
    }

    if (me_opped) {
	for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	    string slnick,slhost;
	    splitprefix(i->mask,slnick,slhost);

	    if (!channels[chan][slnick]) {
		fprintf(*(i->s),"JOIN %s\n",chan.c_str());
	    }
	}

	if (!slaves.size())
	    takeover(f,chan);
	else
	    take_waiting.push_back(chan);
    }

    bool again = 1;
    while (again) {
	again = 0;
	for (vector<string>::iterator i = take_waiting.begin(); i != take_waiting.end(); i++) {
	    if (*i == chan) {
		bool slaves_opped = 1;
		for (slaves_t::iterator j = slaves.begin(); j != slaves.end(); j++) {
		    string slnick,slhost;
		    splitprefix(j->mask,slnick,slhost);

		    if (!channels[chan][slnick]) {
			slaves_opped = 0;
			break;
		    }
		}

		if (slaves_opped) {
		    takeover(f,chan);
		    take_waiting.erase(i);
		    again = 1;
		    break;
		}
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
