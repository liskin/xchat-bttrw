#ifndef IRC_H
#define IRC_H

#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include "../net.h"

namespace std {

struct module;

typedef void (*module_init)(struct module &);
typedef int (*module_config)(string,string);
			    // a      b
typedef void (*module_connected)(FILE*);
typedef void (*module_mode)(FILE*,string,string,string,vector<string>);
				// nick   host   chan   modes
typedef void (*module_msg)(FILE*,string,string,vector<string>);
				// nick  host   cmd
typedef void (*module_cmd)(FILE*,string,vector<string>);
				// nick  cmd list
typedef void (*module_timer)(FILE*);

struct module {
    void *lib;
    module_init init;
    module_config config;
    module_connected connected;
    module_mode mode;
    module_msg msg;
    module_cmd cmd;
    module_timer timer;
};

typedef map<string,module> modules_t;
extern modules_t modules;

extern int port;
extern string server,nick,password,oname,opassword,config,myhost,username,realname,slave_pass;
typedef vector<string> masters_t;
extern masters_t masters;
extern unsigned int max_modes;
extern int safe_mode,some_time,slave_port;
extern bool oper;

extern struct chanmodes {
    string ab; // Always has a parameter. -- YOU MUST WRITE ov(h) HERE!!
    string c;  // Only has a parameter when set.
} isupport;

typedef map<string,string> param_t;
typedef void (*pend_func_t)(FILE*,string&,string&,string&,vector<string>&,param_t&);
typedef map<string,pair<pend_func_t,map<string,string> > > pend_t;
extern pend_t pend;

typedef map<string,string> users_t;
extern users_t users; // nick -> hostmask (a@b)

typedef map<string,bool> channel_t; // nick -> ?op
typedef map<string,channel_t> channels_t; // channel -> list of nicks and ?ops
extern channels_t channels;

// 'slave' connection
struct slave_c {
    bool authd, dead;
    net::TomiTCP *s;
};
typedef vector<slave_c> slavec_t;

// 'slave' (i'm master)
struct slave {
    string mask;
    net::TomiTCP *s;
};
typedef vector<slave> slaves_t;

extern void processsome(FILE *f);
extern void S(FILE *f, const char* fmt, ...);
extern void loadmodule(string name);
extern void unloadmodule(string name);
extern void login(FILE *f);

}

#endif /* IRC_H */
