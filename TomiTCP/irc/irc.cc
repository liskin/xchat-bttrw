#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <fnmatch.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "irc.h"
#include "../str.h"
using namespace std;

namespace std {

// for restart    
char **my_argv;

net::TomiTCP sock, &f = sock /* i'm fuckin' lazy to rewrite almost all of the code */;

string server = ""; int port = 6667;
string nick = "B4gr";
string username = "lama";
string realname = "I did not read anything";
string password = "";
masters_t masters;
string oname = "";
string opassword = "";
string modpath = "..";
string bindhostname = "";
unsigned int max_modes = 4;
int safe_mode = 0; // safe mode, needed for dancer ircd, specifies level of niceness to ircd
int some_time = 300; // time for waiting for some input during operation
struct chanmodes isupport;

int slave_port = 0; // port on which it listens as 'slave'
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
void processsome(net::TomiTCP &f);

void S(net::TomiTCP &f, const char* fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);

    fputs("-> ",stdout);
    vprintf(fmt,ap);
    vfprintf(f,fmt,ap);

    va_end(ap);
}

void addslave(string host, int port, string pass)
{
    slave s;
    s.dead = 0;
    s.maskd = 0;
    s.s = new net::TomiTCP(host,port);
    slaves.push_back(s);
    fprintf(*(s.s),"%s\n",pass.c_str());
}

// host port pass
void confslave(string l)
{
    string host,port,pass;
    stringstream s(l);
    s >> host >> port >> pass;

    if (!host.length() || !port.length()  || !pass.length()) {
	cerr << "Error on line \""+l+"\"" << endl;
	return;
    }

    try {
	addslave(host,atol(port.c_str()),pass);
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }
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
	    unsigned int d = l.find_first_of(" \f\n\r\t\v");
	    if (d > 0 && d != string::npos) {
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
		else if (!strcasecmp(a.c_str(),"bindhostname"))
		    bindhostname = b;
		else if (!strcasecmp(a.c_str(),"module"))
		    loadmodule(b);
		else if (!strcasecmp(a.c_str(),"slave"))
		    confslave(b);
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
	dlclose(m.lib);
	throw runtime_error("Loadmodule error (init): "+string(dlerror()));
    }

    m.init(m);

    modules[name] = m;
}

void login()
{
    if (password.length())
	S(f,"PASS %s\n",password.c_str());
    S(f,"NICK %s\n",nick.c_str());
    S(f,"USER %s 8 * :%s\n",username.c_str(),realname.c_str());
}

void loper()
{
    if (opassword.length() && oname.length())
	S(f,"OPER %s %s\n",oname.c_str(),opassword.c_str());
}

void parsein(const char *buf, string& prefix, vector<string>& cmd)
{
    const char *p = buf, *e = buf + strlen(buf);

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

void splitprefix(string prefix, string &nick, string &host)
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

void parseprefix(string prefix, string &nick, string &host)
{
    splitprefix(prefix,nick,host);
    if (nick.length() && host.length())
	users[nick] = host;
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
		tmp += " " + strtolower(cmd[j]);
		j++;
	    }

	    mode.push_back(tmp);
	}
    }
}

void restart()
{
    S(sock, "QUIT :restarting\n");
    sock.close();
    slave_s.close();
    for (slavec_t::iterator i = slavec.begin(); i != slavec.end(); i++) {
	delete i->s;
    }
    sleep(1);

    execv(my_argv[0],my_argv);
    cerr << "Could not exec!" << endl;
    exit(-1);
}

void docmd(string &snick, string &cmd)
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

    // .addslave host port pass
    if (cl[0] == ".addslave") {
	if (cl.size() != 4)
	    S(f,"PRIVMSG %s :Need 3 parameters\n",snick.c_str());
	else {
	    try {
		addslave(cl[1],atol(cl[2].c_str()),cl[3]);
		S(f,"PRIVMSG %s :slave added\n",snick.c_str());
	    } catch (runtime_error e) {
		S(f,"PRIVMSG %s :addslave error: %s\n",snick.c_str(),e.what());
	    }
	}
	return;
    }

    if (cl[0] == ".listslaves") {
	printf("[1m");
	for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	    if (i->mask.length()) {
		string host = tomi_ntop(i->s->rname);
		int port = ntohs(PORT_SOCKADDR(i->s->rname));
		printf("%s:%i (%s)\n",host.c_str(),port,i->mask.c_str());
	    }
	}
	printf("END of slaves list[0m\n");
    }

    if (cl[0] == ".listslavesirc") {
	for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	    if (i->mask.length()) {
		string host = tomi_ntop(i->s->rname);
		int port = ntohs(PORT_SOCKADDR(i->s->rname));
		S(f,"PRIVMSG %s :%s:%i (%s)\n",snick.c_str(),host.c_str(),port,i->mask.c_str());
	    }
	}
	S(f,"PRIVMSG %s :END of slaves list\n",snick.c_str());
    }

    for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
	if (i->second.cmd)
	    i->second.cmd(snick,cl);
    }
}

void processbuf(const char *buf)
{
    int lastbad = 0;
#define B ({bool ret;if ((time(0)-lastbad)>5){lastbad=time(0);ret=1;}else{ret=0;}ret;})

    string prefix,snick,shost;
    vector<string> cmd;

    parsein(buf,prefix,cmd);
    strtolower(prefix);
    parseprefix(prefix,snick,shost);
    cout << "<- " << buf << endl;

    {
	pend_t::iterator it = pend.find(cmd[0]);
	if (it != pend.end()) {
	    pend_func_t func = it->second.first;
	    param_t param = it->second.second;
	    pend.erase(it);
	    func(prefix,snick,shost,cmd,param);
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
		strtolower(myhost);
	    } else if (n == 1) {
		S(f,"WHOIS %s\n",nick.c_str());
		loper();
		for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
		    if (i->second.connected)
			i->second.connected();
		}
	    } else if (n == 353) {
		stringstream nicks(cmd[4]);
		string nick;
		while (nicks >> nick) {
		    bool op = (nick.length() && nick[0] == '@');
		    nick.erase(0,nick.find_first_not_of("@+"));

		    strtolower(cmd[3]);
		    strtolower(nick);
		    channels[cmd[3]][nick] = op;
		}
	    } else if (n == 352) {
		strtolower(cmd[3]);
		strtolower(cmd[4]);
		strtolower(cmd[6]);
		users[cmd[6]] = cmd[3] + "@" + cmd[4];
	    }
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"PING")) {
	if (cmd.size() >= 2) {
	    S(f,"PONG :%s\n",cmd[1].c_str());
	} else {
	    S(f,"PONG %s\n",nick.c_str());
	}
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
	    docmd(snick,cmd[2]);
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"MODE")) {
	if (cmd.size() >= 3) {
	    vector<string> tcmd(cmd.begin() + 2,cmd.end());
	    vector<string> modes;
	    parsemode(tcmd,modes);
	    strtolower(cmd[1]);

	    // set ?op on the nick in channel
	    for (vector<string>::iterator i = modes.begin(); i != modes.end(); i++) {
		if (i->length() > 3 && (*i)[1] == 'o') {
		    channels[cmd[1]][string(*i,3)] = ((*i)[0] == '+');
		}
	    }

	    for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
		if (i->second.mode)
		    i->second.mode(snick,shost,cmd[1],modes);
	    }
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"JOIN")) {
	strtolower(cmd[1]);
	if (cmd.size() >= 2)
	    channels[cmd[1]][snick] = 0;
	if (!strcasecmp(snick.c_str(),nick.c_str()))
	    S(f,"WHO %s\n",cmd[1].c_str());
    }

    if (!strcasecmp(cmd[0].c_str(),"PART")) {
	if (cmd.size() >= 2) {
	    strtolower(cmd[1]);
	    if (!strcasecmp(snick.c_str(),nick.c_str()))
		channels.erase(cmd[1]);
	    else
		channels[cmd[1]].erase(snick);
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"KICK")) {
	if (cmd.size() >= 3) {
	    strtolower(cmd[1]);
	    strtolower(cmd[2]);
	    if (!strcasecmp(cmd[2].c_str(),nick.c_str()))
		channels.erase(cmd[1]);
	    else
		channels[cmd[1]].erase(cmd[2]);
	}
    }

    if (!strcasecmp(cmd[0].c_str(),"NICK")) {
	if (cmd.size() >= 2) {
	    strtolower(cmd[1]);
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
	    i->second.msg(snick,shost,cmd);
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

void processsome()
{
    string buf;

    // TODO: slave socket and so on...
    while (net::input_timeout(f.sock,some_time) > 0) {
	if (!f.getline(buf))
	    break;
	buf.erase(string(buf).find_last_not_of("\r\n")+1);
	processbuf(buf.c_str());
    }
}

void body()
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
	if (slave_s.ok())
	    FD_SET(slave_s.sock, &set);

	for (slavec_t::iterator i = slavec.begin(); i != slavec.end(); i++) {
	    FD_SET(i->s->sock, &set);
	}
	for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	    FD_SET(i->s->sock, &set);
	}

	int ret = TEMP_FAILURE_RETRY(select(FD_SETSIZE,&set,NULL,NULL,&timeout));
	if (ret == -1)
	    throw runtime_error("error in select: "+string(strerror(errno)));

	// irc server talks
	if (FD_ISSET(f.sock, &set)) {
	    if (! f.getline(buf))
		break;
	    buf.erase(string(buf).find_last_not_of("\r\n")+1);
	    processbuf(buf.c_str());
	}

	// new 'slave' connection
	if (slave_s.ok() && FD_ISSET(slave_s.sock, &set)) {
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
			    fprintf(*(i->s),"%s!%s\n",nick.c_str(),myhost.c_str());
			} else {
			    fprintf(*(i->s),"\nAccess denied\n");
			    i->dead = 1;
			}
		    } else {
			// send his msg to server
			S(f,"%s\n",buf.c_str());
		    }
		}
	    }
	}

	// slave talks
	for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	    if (FD_ISSET(i->s->sock, &set)) {
		if (! i->s->getline(buf)) { // closed
		    cout << "[1mSlave " << tomi_ntop(i->s->rname) << ":" <<
			(int)ntohs(PORT_SOCKADDR(i->s->rname)) << " dropped[0m" << endl;
		    i->dead = 1;
		} else {
		    chomp(buf);
		    if (! i->maskd) { // has not told his hostmask
			i->mask = strtolower(buf);

			bool already = 0;
			for (slaves_t::iterator j = slaves.begin(); j != slaves.end(); j++) {
			    if (j != i && j->mask == i->mask) {
				already = 1;
				break;
			    }
			}
			if (already) {
			    i->dead = 1;
			    cout << "[1mSlave " << tomi_ntop(i->s->rname) << ":" <<
				(int)ntohs(PORT_SOCKADDR(i->s->rname)) <<
				" dropped, already here[0m" << endl;
			} else {
			    i->maskd = 1;
			    cout << "[1mSlave " << tomi_ntop(i->s->rname) << ":" <<
				(int)ntohs(PORT_SOCKADDR(i->s->rname)) << " ready[0m" << endl;
			}
		    } else {
			cout << "slave said: " << buf << endl;
		    }
		}
	    }
	}

	// clean dead 'slave' connections
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

	// clean dead slaves
	clean = 0;
	while (!clean) {
	    clean = 1;
	    for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
		if (i->dead) {
		    delete i->s;
		    clean = 0;
		    slaves.erase(i);
		    break;
		}
	    }
	}

	for (modules_t::iterator i = modules.begin(); i != modules.end(); i++) {
	    if (i->second.timer)
		i->second.timer();
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
	/*for (slaves_t::iterator i = slaves.begin(); i != slaves.end(); i++) {
	    fprintf(*(i->s),"PRIVMSG Liskni_si :baf\n");
	}*/
    }
}

}

int main(int argc, char *argv[])
{
    my_argv = argv;

    srand(time(0) ^ getpid());

    // init structs...
    isupport.ab = "ohvbeIk";
    isupport.c = "l";

    if (argc != 2) {
	cout << "need config file as parameter" << endl;
	return -1;
    }
    try {
	loadconfig(argv[1],cerr);
	if (slave_port)
	    slave_s.listen(slave_port);
    } catch (std::runtime_error e) {
	std::cerr << e.what() << std::endl;
    }

    while (1) {
	sock.close();

	try {
	    sock.connect(server, tostr<int>(port), bindhostname);

	    users.clear();
	    channels.clear();

	    login();
	    body();
	} catch (std::runtime_error e) {
	    std::cerr << e.what() << std::endl;
	}

	sleep(10);
    }

    for (slavec_t::iterator i = slavec.begin(); i != slavec.end(); i++) {
	delete i->s;
    }
    return 0;
}
