#include <iostream>
#include <cstdlib>
#include "xchat.h"
#include "TomiTCP/http.h"

using namespace net;

namespace xchat {
    /**
     * Load list of stars - xchat admins.
     * \return True if the list is sucessfully downloaded.
     */
    void XChat::loadsuperadmins()
    {
	TomiHTTP s;
	string l,superadmin;
	int count;

	int retries = servers.size();
retry:
	try {
	    int ret = s.GET(makeurl("scripts/admin.php"), 0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting stars");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	superadmins.clear();

	if (!s.getline(l)) /* Number of superadmins */
	    throw runtime_error("Getting superadmins info error.");
	count = atoi(l.c_str());
	for (int i = 0; i < count; i++) {
	    if (!s.getline(l))
		throw runtime_error("Superadmins list is not complete.");
	    wstrip(l);
	    superadmin = string(l, 0, l.find(' '));
	    strtolower(superadmin);
	    superadmins.insert(superadmin);
	}
    }
}
