#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"
using namespace net;

namespace xchat {
    /*
     * Init variables, connect to the xchat.cz network.
     */
    XChat::XChat(const string& user, const string& pass)
	: nick(user), last_sent(0), last_recv(0), lastsrv(0)
    {
	// prepare list of available servers
	vector<sockaddr_uni> sockaddrs;
	resolve("xchat.centrum.cz", "", sockaddrs);
	if (! sockaddrs.size())
	    throw runtime_error("Could not get xchat server list from DNS.");
	for (vector<sockaddr_uni>::iterator i = sockaddrs.begin();
		i != sockaddrs.end(); i++)
	    servers.push_back(server(*i));

	TomiHTTP s;
	try {
	    int ret = s.POST(makeurl("~guest~/login/"),
		    "js=1&skin=2&name="+TomiHTTP::URLencode(user)+"&pass="+
		    TomiHTTP::URLencode(pass),0);
	    if (ret != 302)
		throw runtime_error("Not HTTP 302 Found while logging in");
	} catch (runtime_error e) {
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

    /*
     * Part all channels and quit xchat.cz
     */
    XChat::~XChat()
    {
	while (rooms.begin() != rooms.end()) {
	    try { leave(rooms.begin()->first); } catch (...) { }
	}

	TomiHTTP s;
	try {
	    int ret = s.GET(makeurl("~~logout/~$" + uid + "~" + sid + "/"),0);
	    if (ret != 302)
		throw runtime_error("Not HTTP 302 Found while logging off");
	} catch (runtime_error e) {
	    throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}
    }
}
