#ifndef IRC_H
#define IRC_H

#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include "net.h"

namespace std {

typedef void (*module_init)();
typedef int (*module_config)(string,string);
			    // a      b
typedef void (*module_connected)(FILE*);
typedef void (*module_mode)(FILE*,string,string,string,vector<string>);
				// nick   host   chan   modes
typedef void (*module_msg)(FILE*,string,string,vector<string>);
				// nick  host   cmd

struct module {
    void *lib;
    module_init init;
    module_config config;
    module_connected connected;
    module_mode mode;
    module_msg msg;
};

extern map<string,module> modules;

extern int port;
extern string server,nick,password,oname,opassword,config,autotake;
extern vector<string> masters,masternicks;
extern unsigned int max_modes;
extern int safe_mode,some_time;
extern bool oper;

typedef void (*pend_func_t)(FILE*,string&,string&,string&,vector<string>&,map<string,string>&);
typedef map<string,pair<pend_func_t,map<string,string> > > pend_t;
extern pend_t pend;

extern map<string,string> users;
extern map<string,vector<string> > channels;

extern void processsome(FILE *f);
extern void S(FILE *f, const char* fmt, ...);
extern void loadmodule(string name);
extern void unloadmodule(string name);
extern void login(FILE *f);

}

#endif /* IRC_H */
