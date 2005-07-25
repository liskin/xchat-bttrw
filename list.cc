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
    static bool stringcmp(const pair<string,string> &a,
	    const pair<string,string> &b)
    {
	return strcoll(a.second.c_str(), b.second.c_str()) < 0;
    }

    /*
     * Get a list of available rooms
     */
    void XChat::list(listout_t &listout)
    {
	TomiHTTP s;
	string l;

	int retries = servers.size();
retry:
	try {
	    int ret = s.GET(makeurl("~guest~/modchat?op=homepage&skin=2"),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting list");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	while (s.getline(l)) {
	    unsigned int a, b, c;
	    unsigned int pos = 0;
	    static string pat1 = "<option value=\"", pat2 = "\"> ", pat3 = "</option>";

	    while (((a = l.find(pat1, pos)) != string::npos) &&
		    ((b = l.find(pat2, a + pat1.length())) != string::npos) &&
		    ((c = l.find(pat3, b + pat2.length())) != string::npos)) {
		bool fail = 0;
		for (string::iterator i = l.begin() + a + pat1.length();
			i != l.begin() + b; i++)
		    fail |= !isdigit(*i);

		if (!fail) {
		    string rid(l.begin() + a + pat1.length(), l.begin() + b);
		    string name(l.begin() + b + pat2.length(), l.begin() + c);
		    pos = c + pat3.length();

		    striphtmlent(name);
		    listout.push_back(pair<string,string>(rid, name));
		} else {
		    pos = a + 1;
		}
	    }
	}
	s.close();

	sort(listout.begin(), listout.end(), stringcmp);
	for (listout_t::iterator i = listout.begin(); i != listout.end(); i++)
	    i->second = recode_to_client(i->second);
    }
}
