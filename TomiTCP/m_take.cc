#include <iostream>
#include <unistd.h>
#include <fnmatch.h>
#include "irc.h"
using namespace std;

extern "C" {

string autotake;

void takeover(FILE *f, string &prefix, string &snick, string &shost,
	vector<string> &cmd, map<string,string> &param)
{
    if (cmd.size() != 5) {
	cout << "CH\n";
	return;
    }

    if (param["chan"] != cmd[3]) {
	cout << "Ignoring" << endl;
	return;
    }

    if (!strcasestr(cmd[4].c_str(),("@"+nick).c_str())) {
	cout << "Not opped or not in" << endl;
	return;
    }

    if (safe_mode)
	processsome(f);

    vector<string> ops;
    const char *p = cmd[4].c_str(), *e = cmd[4].c_str() + cmd[4].length();

    while (p < e) {
	char* d = strchr(p,' ');
	string n;

	bool no = (*p != '@');

	if (d) {
	    n = string(p+1,0,(d-p-1));
	    p = d+1;
	} else {
	    n = string(p+1);
	    p = e;
	}

	if (!strcasecmp(n.c_str(),nick.c_str()) || no)
	    continue;

	{
	    bool nn = 0;
	    for (vector<string>::iterator it = masternicks.begin(); it!=masternicks.end(); it++)
		if (!fnmatch(it->c_str(),n.c_str(),FNM_CASEFOLD)) {
		    nn = 1;
		    break;
		}
	    if (nn)
		continue;
	}

	ops.push_back(n);
    }

    string out;
    for (unsigned int i=0; i<ops.size(); i+=max_modes) {
	string c = "MODE "+cmd[3]+" -";
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
	    pend["353"].first = takeover;
	    pend["353"].second["chan"] = cl[1];
	    S(f,"NAMES %s\n",cl[1].c_str());
	}
	return;
    }
}

void m_take_mode(FILE *f, string snick, string shost, string chan, vector<string> modes)
{
    if (strcasestr(autotake.c_str(),chan.c_str())) {
	for (vector<string>::iterator it = modes.begin(); it != modes.end(); it++) {
	    if (!strcasecmp(it->c_str(),(string("+o ")+nick).c_str())) {
		pend["353"].first = takeover;
		pend["353"].second["chan"] = chan;
		S(f,"NAMES %s\n",chan.c_str());
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
