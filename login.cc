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
	idle_interval(0), recv_interval(3), convert_smiles(1),
	really_logout(true), watch_global(true), show_history(true),
	client_charset("UTF-8"), note_emitted(false),
	last_superadmins_reload(0)
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

	    loadsuperadmins();
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
	    int ret = request_POST(s, SERVER_MODCHAT, "~guest~/login/", PATH_PLAIN,
                    "js=1&hp=1&usehash=3&skin=2&u_name="+TomiHTTP::URLencode(user)+"&u_pass="+
		    TomiHTTP::URLencode(pass));
	    if (ret != 302)
		throw runtime_error("Not HTTP 302 Found while logging in");

    	    string l = s.headers["location"];

	    string::size_type pos = l.find("xchat.centrum.cz/~$");
	    if (pos == string::npos)
		throw runtime_error("Parse error while logging in (probably bad username or password): " + l);

	    stringstream ss(string(l,pos+19));
	    getline(ss,uid,'~');
	    getline(ss,sid,'/');

	    if (!uid.length() || !sid.length())
		throw runtime_error("Parse error while logging in: " + l);
	} catch (runtime_error &e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

    }

    /**
     * Relogin using saved nick and password.
     */
    void XChat::relogin()
    {
	try {
	    login(me.nick, pass);
	} catch (runtime_error &e) {
	    auto_ptr<EvError> f(new EvError);
	    f->s = e.what();
	    recvq_push((auto_ptr<Event>) f);
	}
    }

    /**
     * Leave all rooms and log out of xchat.cz.
     */
    XChat::~XChat()
    {
	if (!really_logout)
	    return;
    
	while (rooms.begin() != rooms.end()) {
	    try { leave(rooms.begin()->first); } catch (...) { }
	}

	TomiHTTP s;
	
	int retries = servers.size();
retry:
	try {
	    int ret = request_GET(s, SERVER_MODCHAT,
                    "~~logout/~$" + uid + "~" + sid + "/", PATH_PLAIN);
	    if (ret != 302)
		throw runtime_error("Not HTTP 302 Found while logging off");
	} catch (runtime_error &e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    }
	}
    }
}
