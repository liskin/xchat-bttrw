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
	string l;
	int count, pos;

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

    	    superadmin_t superadmin;

	    superadmin.nick = string(l, 0, l.find(' '));
	    l.erase(0, superadmin.nick.length());
	    l.erase(0, l.find_first_not_of(' '));

	    pos = l.find(' ');
	    superadmin.sex = (string(l, 0, pos) == "0") ? 1 : 0;
	    l.erase(0, pos);
	    l.erase(0, l.find_first_not_of(' '));

	    pos = l.find(' ');
	    superadmin.star = atoi(string(l, 0, pos).c_str());
	    if (superadmin.star < 2 || superadmin.star > 4)
		throw("Bad star ID, please contact developer team!");
	    l.erase(0, pos);
	    l.erase(0, l.find_first_not_of(' '));

	    superadmin.online = (string(l, 0, l.find(' ')) == "1");

	    string nick = superadmin.nick;
	    strtolower(nick);

	    superadmins[nick] = superadmin;
	}
    }
    
    /**
     * Reload superadmins, check for changes and emit appropriate events.
     */
    void XChat::reloadsuperadmins()
    {
	superadmins_t old = superadmins;
	loadsuperadmins();

	/*
	 * Check for super admins changes
	 */
	vector<string> added, removed;

	superadmins_t::iterator o = old.begin(), oe = old.end(),
	    n = superadmins.begin(), ne = superadmins.end();

	while (o != oe && n != ne)
	    if (o->first < n->first)
		removed.push_back(o++->first);
	    else if (n->first < o->first)
		added.push_back(n++->first);
	    else
		o++, n++;
	while (o != oe)
	    removed.push_back(o++->first);
	while (n != ne)
	    added.push_back(n++->first);

	if (added.size() || removed.size()) {
	    EvSuperAdminsChange *e = new EvSuperAdminsChange;
	    e->added = added;
	    e->removed = removed;
	    recvq_push(e);
	}
    }    
}
