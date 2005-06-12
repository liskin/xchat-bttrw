#include <iostream>
#include <cstdlib>
#include "xchat.h"
#include "TomiTCP/http.h"

using namespace net;

namespace xchat {
    /*
     * Check, if a nick is online.
     */
    bool XChat::ison(const string& nick)
    {
	TomiHTTP s;

	int retries = servers.size();
retry:
	try {
	    int ret = s.GET(makeurl("scripts/online_txt.php?nick=" + 
			TomiHTTP::URLencode(nick)),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting online status");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	string l;
	s.getline(l);

	if (l == "1")
	    return true;
	if (l == "0")
	    return false;

	throw runtime_error("Unexpected online status reply, please contact developer team!");
    }
}
