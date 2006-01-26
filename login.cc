#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"
using namespace net;

namespace xchat {
    /**
     * Init variables, connect to the xchat.cz network.
     * \param user Nick name.
     * \param pass Password.
     */
    XChat::XChat(const string& user, const string& pass)
	: lastsrv(0), last_sent(0), last_recv(0), idle_delta(0),
	idle_interval(0), recv_interval(3), convert_smiles(true)
    {
	/*
	 * This is here, because otherwise every forked child would have the
	 * same RNG state.
	 */
	srand(time(0) ^ getpid());
	makeservers();

	login(user, pass);

	this->pass = pass;
	me.nick = user;
	me.sex = 2;
	try {
	    userinfo_t ui = userinfo(user);
	    if (ui.nick.length())
		me.sex = ui.sex;
	} catch (...) {	}
    }

    /**
     * Log in to xchat.cz.
     * \param user Nick name.
     * \param pass Password.
     */
    void XChat::login(const string& user, const string& pass)
    {
	TomiHTTP s;

	int retries = servers.size();
retry:
	try {
	    int ret = s.POST(makeurl("~guest~/login/"),
		    "js=1&hp=1&usehash=3&skin=2&u_name="+TomiHTTP::URLencode(user)+"&u_pass="+
		    TomiHTTP::URLencode(pass),0);
	    if (ret != 302)
		throw runtime_error("Not HTTP 302 Found while logging in");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	string l = s.headers["location"];

	unsigned int pos = l.find("xchat.centrum.cz/~$");
	if (pos == string::npos)
	    throw runtime_error("Parse error while logging in: " + l);

	stringstream ss(string(l,pos+19));
	getline(ss,uid,'~');
	getline(ss,sid,'/');

	if (!uid.length() || !sid.length())
	    throw runtime_error("Parse error while logging in: " + l);
    }

    /**
     * Relogin using saved nick and password.
     */
    void XChat::relogin()
    {
	try {
	    login(me.nick, pass);
	} catch (runtime_error e) {
	    EvError *f = new EvError;
	    f->s = e.what();
	    recvq_push(f);
	}
    }

    /**
     * Leave all rooms and log out of xchat.cz.
     */
    XChat::~XChat()
    {
	while (rooms.begin() != rooms.end()) {
	    try { leave(rooms.begin()->first); } catch (...) { }
	}

	TomiHTTP s;
	
	int retries = servers.size();
retry:
	try {
	    int ret = s.GET(makeurl("~~logout/~$" + uid + "~" + sid + "/"),0);
	    if (ret != 302)
		throw runtime_error("Not HTTP 302 Found while logging off");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}
    }

    /**
     * Get the list of available servers from DNS.
     * If not possible (in proxy environments for example), add
     * "xchat.centrum.cz" (clustering and failure tracking won't work).
     */
    void XChat::makeservers()
    {
	int i;
	
	for (char *c = "sp"; *c; c++) {
	    for (i = 1; i <= 100 /* safety limit */; i++) {
		vector<sockaddr_uni> sockaddrs;
	        try {
		    resolve(*c + string("x") + tostr<int>(i) + ".xchat.cz", "", sockaddrs);
		} catch (...) { }

		if (!sockaddrs.size())
		    break;

		for (vector<sockaddr_uni>::iterator i = sockaddrs.begin();
			i != sockaddrs.end(); i++)
		    servers.push_back(server(tomi_ntop(*i)));
	    }
	}

	if (servers.empty()) {
	    servers.push_back(server("xchat.centrum.cz"));

	    EvError *e = new EvError;
	    e->s = "Could not get xchat server list from DNS. Using xchat.centrum.cz as host.";
	    recvq_push(e);
	}
    }
}
