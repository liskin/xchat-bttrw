#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

namespace xchat {
    static bool stringcmp(const roominfo &a,
	    const roominfo &b)
    {
	return strcoll(a.name.c_str(), b.name.c_str()) < 0;
    }

    /**
     * Get a list of available rooms.
     * \param listout Output array.
     */
    void XChat::list(listout_t &listout)
    {
	TomiHTTP s;
	string l;

	int retries = servers.size();
retry:
	try {
	    int ret = s.GET(makeurl("~guest~/modchat?op=wwpageng&skin=2"),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting list");
	} catch (runtime_error &e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	while (s.getline(l)) {
	    string::size_type a, b, c, d;
	    string::size_type pos = 0;
	    static string pat1 = "<option value=\"", pat2 = "\" >", 
		pat3 = " (", pat4 = ")</option>";

	    while (((a = l.find(pat1, pos)) != string::npos) &&
		    ((b = l.find(pat2, a + pat1.length())) != string::npos) &&
		    ((c = l.find(pat3, b + pat2.length())) != string::npos) &&
		    ((d = l.find(pat4, c + pat3.length())) != string::npos)) {
		bool fail = 0;
		for (string::iterator i = l.begin() + a + pat1.length();
			i != l.begin() + b; i++)
		    fail |= !isdigit(*i);
		
		if (!fail) {
		    string rid(l.begin() + a + pat1.length(), l.begin() + b);
		    string name(l.begin() + b + pat2.length(), l.begin() + c);
		    string count(l.begin() + c + pat3.length(), l.begin() + d);
		    pos = d + pat4.length();

		    striphtmlent(name);

		    roominfo room;
		    room.rid = rid;
		    room.name = name;
		    room.count = atoi(count.c_str());
		    listout.push_back(room);
		} else {
		    pos = a + 1;
		}
	    }
	}
	s.close();

	sort(listout.begin(), listout.end(), stringcmp);
	for (listout_t::iterator i = listout.begin(); i != listout.end(); i++)
	    i->name = recode_to_client(i->name);
    }
}
