#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <fnmatch.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include "net.h"
#include "irc.h"
using namespace std;

namespace std {

// for restart    
char **my_argv;
FILE *my_f;

string server = ""; int port = 6667;
string nick = "B4gr";
string password = "";
vector<string> masters;
vector<string> masternicks;
string oname = "";
string opassword = "";
string modpath = "/home/pytt_l/c++/TomiTCP";
unsigned int max_modes = 4;
int safe_mode = 0; // safe mode, needed for dancer ircd, specifies level of niceness to ircd
int some_time = 300; // time for waiting for some input during operation

bool oper = 0;

string config;

string autotake = "";

pend_t pend;

map<string,string> users;
map<string,vector<string> > channels;

map<string,module> modules;

// forward declarations
void processsome(FILE *f);

void S(FILE *f, const char* fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);

    fputs("-> ",stdout);
    vprintf(fmt,ap);
    vfprintf(f,fmt,ap);

    va_end(ap);
}

inline string& wstrip(string &str)
{
    str.erase(0,str.find_first_not_of(" \f\n\r\t\v"));
    str.erase(str.find_last_not_of(" \f\n\r\t\v")+1);
    return str;
}

void loadconfig(const char *fname, ostream &out)
{
    ifstream in(fname);
    if (!in.is_open()) {
	throw runtime_error("Could not open config file");
    }

    config = fname;

    string l;
    while (getline(in,l)) {
	if (l.length() && l[0]!='#') {
	    int d = l.find_first_of(" \f\n\r\t\v");
	    if (d > 0) {
		string a(l,0,d),b(l,d+1);
		wstrip(a); wstrip(b);
		if (!strcasecmp(a.c_str(),"server"))
		    server = b;
		else if (!strcasecmp(a.c_str(),"port"))
		    port = atol(b.c_str());
		else if (!strcasecmp(a.c_str(),"nick"))
		    nick = b;
		else if (!strcasecmp(a.c_str(),"password"))
		    password = b;
		else if (!strcasecmp(a.c_str(),"master"))
		    masters.push_back(b);
		else if (!strcasecmp(a.c_str(),"masternick"))
		    masternicks.push_back(b);
		else if (!strcasecmp(a.c_str(),"oname"))
		    oname = b;
		else if (!strcasecmp(a.c_str(),"opassword"))
		    opassword = b;
		else if (!strcasecmp(a.c_str(),"max_modes"))
		    max_modes = atol(b.c_str());
		else if (!strcasecmp(a.c_str(),"safe_mode"))
		    safe_mode = atol(b.c_str());
		else if (!strcasecmp(a.c_str(),"some_time"))
		    some_time = atol(b.c_str());
		else if (!strcasecmp(a.c_str(),"modpath"))
		    modpath = b;
		else if (!strcasecmp(a.c_str(),"module"))
		    loadmodule(b);
		else {
		    bool ok = 0;
		    for (map<string,module>::iterator i = modules.begin(); i != modules.end(); i++) {
			if (i->second.config && !i->second.config(a,b)) {
			    ok = 1;
			    break;
			}
		    }
		    if (!ok) {
			out << "Error on line \""+l+"\"" << endl;
		    }
		}
	    } else {
		out << "Error on line \""+l+"\"" << endl;
	    }
	}
    }

    if (!server.length())
	throw runtime_error("Server not defined");
    if (!port)
	throw runtime_error("Port not defined");
    if (!nick.length())
	throw runtime_error("Nick not defined");
    if (!masters.size())
	throw runtime_error("No masters defined");
    if (!masternicks.size())
	throw runtime_error("No master nicks defined");
    if (!max_modes)
	throw runtime_error("Max_modes not defined");
    if (!some_time)
	out << "some_time = 0, safe mode may not work properly" << endl;
    if (safe_mode)
	out << "Using safe mode" << endl;
}

void unloadmodule(string name)
{
    map<string,module>::iterator i = modules.find(name);
    if (i != modules.end()) {
	dlclose(i->second.lib);
	modules.erase(i);
    }
}

void loadmodule(string name)
{
    unloadmodule(name);

    module m;
    memset(&m,0,sizeof(m));

    m.lib = dlopen((modpath+"/m_"+name+".so").c_str(),RTLD_NOW | RTLD_GLOBAL);
    if (!m.lib) {
	throw runtime_error("Loadmodule error: "+string(dlerror()));
    }

    m.init = (module_init) dlsym(m.lib,("m_"+name+"_init").c_str());
    if (!m.init) {
	throw runtime_error("Loadmodule error (init): "+string(dlerror()));
    }

    m.init(m);

    modules[name] = m;
}

void login(FILE *f)
{
    if (password.length())
	S(f,"PASS %s\n",password.c_str());
    S(f,"NICK %s\n",nick.c_str());
    S(f,"USER %s 8 * :%s\n",nick.c_str(),nick.c_str());
}

void loper(FILE *f)
{
    if (opassword.length() && oname.length())
	S(f,"OPER %s %s\n",oname.c_str(),opassword.c_str());
}

void parsein(char *buf, string& prefix, vector<string>& cmd)
{
    char *p = buf, *e = buf + strlen(buf);

    buf[string(buf).find_last_not_of("\r\n")+1] = 0;

    if (buf[0] == ':') {
	p++;
	char* d = strchr(p,' ');
	if (d) {
	    prefix = string(p,0,(d-p));
	    p = d+1;
	} else {
	    prefix = p;
	    p = e;
	}
    }

    while (p < e) {
	if (p[0] == ':') {
	    cmd.push_back(string(p+1));
	    p = e;
	} else {
	    char* d = strchr(p,' ');
	    if (d) {
		cmd.push_back(string(p,0,(d-p)));
		p = d+1;
	    } else {
		cmd.push_back(string(p));
		p = e;
	    }
	}
    }
}

void parseprefix(string &prefix, string &nick, string &host)
{
    int d = prefix.find('!');
    if (d == -1) {
	nick = "";
	host = "";
    } else {
	nick = string(prefix,0,d);
	host = string(prefix,d+1);
    }
}

void parsecmd(const char *buf, vector<string>& cmd)
{
    const char *p = buf, *e = buf + strlen(buf);

    while (p < e) {
	if (p[0] == ':') {
	    cmd.push_back(string(p+1));
	    p = e;
	} else {
	    char* d = strchr(p,' ');
	    if (d) {
		cmd.push_back(string(p,0,(d-p)));
		p = d+1;
	    } else {
		cmd.push_back(string(p));
		p = e;
	    }
	}
    }
}

void parsemode(vector<string>& cmd, vector<string>& mode)
{
    char ch[2] = "+",t[2] = " ";
    unsigned int j=1;

    for (string::iterator i=cmd[0].begin(); i!=cmd[0].end(); i++) {
	if (j >= cmd.size())
	    break;
	if (*i == '+' || *i == '-')
	    ch[0] = *i;
	else {
	    t[0] = *i;
	    mode.push_back(string(ch)+string(t)+" "+cmd[j]);
	    j++;
	}
    }
}

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

void restart()
{
    S(my_f,"QUIT :restarting\n");
    fclose(my_f);
    sleep(1);

    execv(my_argv[0],my_argv);
    cerr << "Could not exec!" << endl;
    exit(-1);
}

void docmd(FILE *f, string &snick, string &cmd)
{
    if (string(cmd,0,3) == ".q " && cmd.length() > 3) {
	S(f,"%s\n",cmd.c_str() + 3);
	return;
    }

    if (string(cmd,0,2) == ".q") {
	S(f,"PRIVMSG %s :Have a nice day\n",snick.c_str());
	S(f,"QUIT :shutdown by master\n");
	fclose(f);
	throw runtime_error("shutdown by master");
    }

    vector<string> cl;
    parsecmd(cmd.c_str(),cl);
    if (!cl.size())
	return;

    if (cl[0] == "hello") {
	S(f,"PRIVMSG %s :Heya', here %s, up and running\n",snick.c_str(),nick.c_str());
	return;
    }

    if (cl[0] == ".m") {
	if (cl.size() != 3)
	    S(f,"PRIVMSG %s :Need 2 parameters\n",snick.c_str());
	else
	    S(f,"PRIVMSG %s :%s\n",cl[1].c_str(),cl[2].c_str());
	return;
    }

    if (cl[0] == ".mm") {
	if (cl.size() != 4) {
	    S(f,"PRIVMSG %s :Need 3 parameters\n",snick.c_str());
	} else {
	    string out;
	    for (int i=0; i<atol(cl[1].c_str()); i++)
		out += "PRIVMSG " + cl[2] + " :" + cl[3] + "\n";
	    S(f,"%s",out.c_str());
	}
    }

    if (cl[0] == ".j") {
	if (cl.size() != 2)
	    S(f,"PRIVMSG %s :Need 1 parameters\n",snick.c_str());
	else
	    S(f,"JOIN %s\n",cl[1].c_str());
	return;
    }

    if (cl[0] == ".p") {
	if (cl.size() != 2)
	    S(f,"PRIVMSG %s :Need 1 parameters\n",snick.c_str());
	else
	    S(f,"PART %s :have a nice day\n",cl[1].c_str());
	return;
    }

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

    if (cl[0] == ".rehash") {
	if (cl.size() != 1) {
	    S(f,"PRIVMSG %s :Need no parameters\n",snick.c_str());
	} else {
	    S(f,"PRIVMSG %s :Rehashing...\n",snick.c_str());
	    try {
		string oldnick = nick;

		stringstream ss;
		loadconfig(config.c_str(),ss);

		string l;
		while (getline(ss,l)) {
		    S(f,"PRIVMSG %s :Rehash warning: %s\n",snick.c_str(),l.c_str());
		    sleep(1); // avoid excess flood
		}

		S(f,"PRIVMSG %s :Rehash done\n",snick.c_str());

		if (nick != oldnick) {
		    S(f,"PRIVMSG %s :Changing nick to %s\n",snick.c_str(),nick.c_str());
		    S(f,"NICK %s\n",nick.c_str());
		}
	    } catch (std::runtime_error e) {
		S(f,"PRIVMSG %s :Rehash error: %s\n",snick.c_str(),e.what());
	    }
	}
	return;
    }

    if (cl[0] == ".restart") {
	if (cl.size() != 1) {
	    S(f,"PRIVMSG %s :Need no parameters\n",snick.c_str());
	} else {
	    S(f,"PRIVMSG %s :Restarting...\n",snick.c_str());
	    restart();
	    throw runtime_error("Could not restart :(");
	}
	return;
    }
}

void processbuf(FILE *f, char *buf)
{
    int lastbad = 0;
#define B ({bool ret;if ((time(0)-lastbad)>5){lastbad=time(0);ret=1;}else{ret=0;}ret;})

    string prefix,snick,shost;
    vector<string> cmd;

    parsein(buf,prefix,cmd);
    parseprefix(prefix,snick,shost);
    cout << "<- " << buf << endl;

    {
	pend_t::iterator it = pend.find(cmd[0]);
	if (it != pend.end()) {
	    pend_func_t func = it->second.first;
	    map<string,string> param = it->second.second;
	    pend.erase(it);
	    func(f,prefix,snick,shost,cmd,param);
	}
    }

    {
	int n;
	if ((n = atol(cmd[0].c_str()))) {
	    if (n == 433) {
		cout << "Nick busy" << endl;
		nick[nick.length()-1]++;
		S(f,"NICK %s\n",nick.c_str());
	    }
	    if (n == 431 || n == 436 || /*n == 451 ||*/ n == 464 || n == 465) {
		fclose(f);
		cout << "Server returned fatal error" << endl;
	    }
	    if (n == 381) {
		oper = 1;
	    }
	    if (n == 1) {
		loper(f);
		for (map<string,module>::iterator i = modules.begin(); i != modules.end(); i++) {
		    if (i->second.connected)
			i->second.connected(f);
		}
	    }
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"PING")) {
	S(f,"PONG %s\n",nick.c_str());
    }

    if ((!strcasecmp(cmd[0].c_str(),"PRIVMSG")) && (!strcasecmp(cmd[1].c_str(),nick.c_str()))
	    && strcasecmp(snick.c_str(),nick.c_str())) {
	bool nn = 0;
	for (vector<string>::iterator it = masters.begin(); it!=masters.end(); it++)
	    if (!fnmatch(it->c_str(),prefix.c_str(),FNM_CASEFOLD)) {
		nn = 1;
		break;
	    }

	if (!nn) {
	    if (B) S(f,"PRIVMSG %s :Unpriv\n",snick.c_str());
	} else {
	    docmd(f,snick,cmd[2]);
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"MODE")) {
	if (cmd.size() >= 4) {
	    vector<string> tcmd(cmd.begin() + 2,cmd.end());
	    vector<string> modes;
	    parsemode(tcmd,modes);

	    if (strcasestr(autotake.c_str(),cmd[1].c_str())) {
		for (vector<string>::iterator it = modes.begin(); it != modes.end(); it++) {
		    if (!strcasecmp(it->c_str(),(string("+o ")+nick).c_str())) {
			pend["353"].first = takeover;
			pend["353"].second["chan"] = cmd[1];
			S(f,"NAMES %s\n",cmd[1].c_str());
			break;
		    }
		}
	    }

	    for (map<string,module>::iterator i = modules.begin(); i != modules.end(); i++) {
		if (i->second.mode)
		    i->second.mode(f,snick,shost,cmd[1],modes);
	    }
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"JOIN")) {
    }

    for (map<string,module>::iterator i = modules.begin(); i != modules.end(); i++) {
	if (i->second.msg)
	    i->second.msg(f,snick,shost,cmd);
    }
}

void processsome(FILE *f)
{
    char buf[4096];

    while (net::input_timeout(fileno(f),300) > 0) {
	fgets(buf,4096,f);
	processbuf(f,buf);
    }
}

void body(FILE *f)
{
    char buf[4096];

    while (fgets(buf,4096,f)) {
	processbuf(f,buf);
    }
}

}

int main(int argc, char *argv[])
{
    my_argv = argv;

    if (argc != 2) {
	cout << "need config file as parameter" << endl;
	return -1;
    }
    try {
	loadconfig(argv[1],cerr);

	while (1) {
	    net::TomiTCP sock(server,port);
	    FILE *f = sock.makestream();
	    my_f = f;

	    login(f);
	    body(f);
	    fclose(f);
	    sleep(10);
	}
    } catch (std::runtime_error e) {
	std::cerr << e.what() << std::endl;
    }
    return 0;
}
