#include <cstdio>
#include <cstdlib>
#include <stdbool.h>
#include <iostream>
#include "xchat.h"
#include "smiles.h"
#include "idle.h"
#include "TomiTCP/str.h"

namespace xchat {
    /*
     * Variable definitions
     */
    int idle_interval = 0;

    /*
     * Prepare URL with given full path.
     */
    string XChat::makeurl(const string& path)
    {
	return "http://" + tomi_ntop(servers[rand() % servers.size()]) + "/" + path;
    }

    /*
     * Prepare URL with given the path which should be appended to uid and sid
     */
    string XChat::makeurl2(const string& path)
    {
	return "http://" + tomi_ntop(servers[rand() % servers.size()]) + "/~$" +
	    uid + "~" + sid + "/" + path;
    }

    /*
     * Find a nick structure in rooms we are
     */
    x_nick* XChat::findnick(string src, room **r)
    {
	strtolower(src);
	for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
	    nicklist_t::iterator n = i->second.nicklist.find(src);
	    if (n != i->second.nicklist.end()) {
		if (r)
		    *r = &i->second;
		return &n->second;
	    }
	}

	return 0;
    }

    /*
     * Get a host for user based on his/her sex
     */
    const char * XChat::getsexhost(string src)
    {
	strtolower(src);

	for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++)
	    if (i->second.nicklist.find(src) != i->second.nicklist.end())
		return sexhost[i->second.nicklist[src].sex];

	return sexhost[2];
    }

    /*
     * Go through sendq and send messages, take flood protection into account.
     * Then, send anti-idle messages if necessary.
     */
    void XChat::do_sendq()
    {
	if (!sendq.empty() && time(0) - last_sent >= send_interval) {
	    pair<string,string> msg = sendq.front(); sendq.pop();
	    putmsg(rooms[msg.first], msg.second);
	}

	// f00king idler
	if (idle_interval && sendq.empty())
	    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
		if (time(0) - i->second.last_sent >= idle_interval) {
		    sendq_push(i->first, "/s " + nick + " " + genidle());
		}
	    }
    }
}
