#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <fnmatch.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include "irc.h"
#include "../str.h"
using namespace std;

namespace std {

// for restart    
char **my_argv;
FILE *my_f;

string server = ""; int port = 6667;
string nick = "B4gr";
string username = "lama";
string realname = "I did not read anything";
string password = "";
masters_t masters;
string oname = "";
string opassword = "";
string modpath = "/home/pytt_l/c++/TomiTCP/irc";
unsigned int max_modes = 4;
int safe_mode = 0; // safe mode, needed for dancer ircd, specifies level of niceness to ircd
int some_time = 300; // time for waiting for some input during operation
struct chanmodes isupport;

int slave_port = 12205; // port on which it listens as 'slave'
string slave_pass = "scosux"; // password for 'slave'

net::TomiTCP slave_s;
slavec_t slavec;
slaves_t slaves;

bool oper = 0;
string myhost;

string config;

pend_t pend;

users_t users;
channels_t channels;

modules_t modules;

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
		else if (!strcasecmp(a.c_str(),"username"))
		    username = b;
		else if (!strcasecmp(a.c_str(),"realname"))
		    realname = b;
		else if (!strcasecmp(a.c_str(),"master"))
		    masters.push_back(b);
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
		else if (!strcasecmp(a.c_str(),"isupport_ab"))
		    isupport.ab = b;
		else if (!strcasecmp(a.c_str(),"isupport_c"))
		    isupport.c = b;
		else if (!strcasecmp(a.c_str(),"slave_port"))
		    slave_port = atol(b.c_str());
		else if (!strcasecmp(a.c_str(),"slave_pass"))
		    slave_pass = b;
		else if (!strcasecmp(a.c_str(),"module"))
		    loadmodule(b);
		else {
		    bool ok = 0;
		    for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
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
    if (!username.length())
	throw runtime_error("Username not defined");
    if (!masters.size())
	throw runtime_error("No masters defined");
    if (!max_modes)
	throw runtime_error("Max_modes not defined");
    if (!some_time)
	out << "some_time = 0, safe mode may not work properly" << endl;
    if (safe_mode)
	out << "Using safe mode" << endl;
}

void unloadmodule(string name)
{
    modules_t::iterator i = modules.find(name);
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
    S(f,"USER %s 8 * :%s\n",username.c_str(),realname.c_str());
}

void loper(FILE *f)
{
    if (opassword.length() && oname.length())
	S(f,"OPER %s %s\n",oname.c_str(),opassword.c_str());
}

void parsein(const char *bufa, string& prefix, vector<string>& cmd)
{
    char buf[strlen(bufa)];
    const char *p = buf, *e = buf + strlen(buf);

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

	// this shouldn't really be here :)
	users[nick] = host;
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
	if (*i == '+' || *i == '-')
	    ch[0] = *i;
	else {
	    t[0] = *i;
	    string tmp = string(ch)+string(t);

	    // has parameter?
	    if (j < cmd.size() && ((isupport.ab.find(t[0]) != string::npos) ||
		    (isupport.c.find(t[0]) != string::npos && ch[0] == '+'))) {
		tmp += " " + cmd[j];
		j++;
	    }

	    mode.push_back(tmp);
	}
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

    for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
	if (i->second.cmd)
	    i->second.cmd(f,snick,cl);
    }
}

void processbuf(FILE *f, const char *buf)
{
    int lastbad = 0;
#define B ({bool ret;if ((time(0)-lastbad)>5){lastbad=time(0);ret=1;}else{ret=0;}ret;})

    string prefix,snick,shost;
    vector<string> cmd;

    parsein(buf,prefix,cmd);
    parseprefix(prefix,snick,shost);
    cout << "<- " << buf;

    {
	pend_t::iterator it = pend.find(cmd[0]);
	if (it != pend.end()) {
	    pend_func_t func = it->second.first;
	    param_t param = it->second.second;
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
	    } else if (n == 431 || n == 436 || /*n == 451 ||*/ n == 464 || n == 465) {
		fclose(f);
		cout << "Server returned fatal error" << endl;
	    } else if (n == 381) {
		oper = 1;
	    } else if (n == 311) {
		myhost = cmd[3]+"@"+cmd[4];
		cout << myhost << endl;
	    } else if (n == 1) {
		S(f,"WHOIS %s\n",nick.c_str());
		loper(f);
		for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
		    if (i->second.connected)
			i->second.connected(f);
		}
	    } else if (n == 353) {
		stringstream nicks(cmd[4]);
		string nick;
		while (nicks >> nick) {
		    bool op = (nick.length() && nick[0] == '@');
		    nick.erase(0,nick.find_first_not_of("@+"));

		    channels[cmd[3]][nick] = op;
		}
	    } else if (n == 352) {
		users[cmd[6]] = cmd[3] + "@" + cmd[4];
	    }
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"PING")) {
	S(f,"PONG %s\n",nick.c_str());
    }

    if ((!strcasecmp(cmd[0].c_str(),"PRIVMSG")) && (!strcasecmp(cmd[1].c_str(),nick.c_str()))
	    && strcasecmp(snick.c_str(),nick.c_str())) {
	bool nn = 0;
	for (masters_t::iterator it = masters.begin(); it!=masters.end(); it++)
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

	    // set ?op on the nick in channel
	    for (vector<string>::iterator i = modes.begin(); i != modes.end(); i++) {
		if (i->length() > 3 && (*i)[1] == 'o') {
		    channels[cmd[1]][string(*i,3)] = ((*i)[0] == '+');
		}
	    }

	    for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
		if (i->second.mode)
		    i->second.mode(f,snick,shost,cmd[1],modes);
	    }
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"JOIN")) {
	if (cmd.size() >= 2)
	    channels[cmd[1]][snick] = 0;
	if (snick == nick)
	    S(f,"WHO %s\n",cmd[1].c_str());
    }

    if (!strcasecmp(cmd[0].c_str(),"PART")) {
	if (cmd.size() >= 2) {
	    if (snick == nick)
		channels.erase(cmd[1]);
	    else
		channels[cmd[1]].erase(snick);
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"KICK")) {
	if (cmd.size() >= 3) {
	    if (cmd[2] == nick)
		channels.erase(cmd[1]);
	    else
		channels[cmd[1]].erase(cmd[2]);
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"NICK")) {
	if (cmd.size() >= 2) {
	    users.erase(snick);
	    users[cmd[1]] = shost;

	    // change nick in channels
	    for (channels_t::iterator i = channels.begin(); i != channels.end(); i++) {
		channel_t::iterator j = i->second.find(snick);
		if (j != i->second.end()) {
		    bool op = j->second;
		    i->second.erase(j->first);

		    i->second[cmd[1]] = op;
		}
	    }
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"QUIT")) {
	users.erase(snick);

	for (channels_t::iterator i = channels.begin(); i != channels.end(); i++) {
	    i->second.erase(snick);
	}
    }

    for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
	if (i->second.msg)
	    i->second.msg(f,snick,shost,cmd);
    }
}

void users_clean()
{
    users_t old = users;
    users.clear();

    for (channels_t::iterator i = channels.begin(); i != channels.end(); i++) {
	for (channel_t::iterator j = i->second.begin(); j != i->second.end(); j++) {
	    users[j->first] = old[j->first];
	}
    }
}

void processsome(FILE *f)
{
    char buf[4096];

    // TODO: slave socket and so on...
    while (net::input_timeout(fileno(f),some_time) > 0) {
	fgets(buf,4096,f);
	processbuf(f,buf);
    }
}

void body(net::TomiTCP &f)
{
    string buf;
    int iter = 0;

    while (1) {
	iter++;

	fd_set set;
	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	FD_ZERO(&set);
	FD_SET(f.sock, &set);
	FD_SET(slave_s.sock, &set);
	for (slavec_t::iterator i = slavec.begin(); i != slavec.end(); i++) {
	    FD_SET(i->s->sock, &set);
	}

	int ret = TEMP_FAILURE_RETRY(select(FD_SETSIZE,&set,NULL,NULL,&timeout));
	if (ret == -1)
	    throw runtime_error("error in select: "+string(strerror(errno)));

	// irc server talks
	if (FD_ISSET(f.sock, &set)) {
	    if (! f.getline(buf))
		break;
	    processbuf(f,buf.c_str());
	}

	// new 'slave' connection
	if (FD_ISSET(slave_s.sock, &set)) {
	    slave_c c;
	    c.authd = 0;
	    c.dead = 0;
	    c.s = slave_s.accept();

	    slavec.push_back(c);
	}

	// 'slave' connection talks
	for (slavec_t::iterator i = slavec.begin(); i != slavec.end(); i++) {
	    if (FD_ISSET(i->s->sock, &set)) {
		if (! i->s->getline(buf)) { // closed
		    i->dead = 1;
		} else {
		    chomp(buf);
		    if (! i->authd) { // has not authenticated
			if (buf == slave_pass) {
			    i->authd = 1;
			} else {
			    fprintf(*(i->s),"Access denied\n");
			    i->dead = 1;
			}
		    } else {
			// send his msg to server
			S(f,"%s\n",buf.c_str());
		    }
		}
	    }
	}

	bool clean = 0;
	while (!clean) {
	    clean = 1;
	    for (slavec_t::iterator i = slavec.begin(); i != slavec.end(); i++) {
		if (i->dead) {
		    delete i->s;
		    clean = 0;
		    slavec.erase(i);
		    break;
		}
	    }
	}

	for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
	    if (i->second.timer)
		i->second.timer(f);
	}

	// clean users array every 100th iteration
	if (iter > 100) {
	    users_clean();
	    iter = 0;
	}

	/*cout << "---\n";
	for (map<string,string>::iterator i = users.begin(); i != users.end(); i++) {
	    cout << i->first << "!" << i->second << endl;
	}
	cout << "---\n";*/
	/*cout << "---\n";
	for (channel_t::iterator i = channels["#nomi"].begin(); i != channels["#nomi"].end(); i++) {
	    cout << i->first << " - op? " << (int)i->second << endl;
	}
	cout << "---\n";*/
    }
}

}

int main(int argc, char *argv[])
{
    my_argv = argv;

    // init structs...
    isupport.ab = "ohvbeIk";
    isupport.c = "l";

    if (argc != 2) {
	cout << "need config file as parameter" << endl;
	return -1;
    }
    try {
	loadconfig(argv[1],cerr);
	slave_s.listen(slave_port);

	while (1) {
	    net::TomiTCP sock(server,port);
	    FILE *f = sock;
	    my_f = f;

	    users.clear();
	    channels.clear();

	    login(f);
	    body(sock);
	    sleep(10);
	}
    } catch (std::runtime_error e) {
	std::cerr << e.what() << std::endl;
	for (slavec_t::iterator i = slavec.begin(); i != slavec.end(); i++) {
	    delete i->s;
	}
    }
    return 0;
}
