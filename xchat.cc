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
    string client_charset;

    /*
     * Init rng and recode.
     */
    void xchat_init()
    {
	srand(time(0) ^ getpid());
	init_recode();
    }

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
     * Go through sendq and send messages, take flood protection into account.
     * Then, send anti-idle messages if necessary.
     */
    void XChat::do_sendq()
    {
	if (!sendq.empty() && time(0) - last_sent >= send_interval) {
	    pair<string,string> msg = sendq.front(); sendq.pop();
	    if (rooms.find(msg.first) != rooms.end())
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

    /*
     * Get new messages if it should be done yet.
     */
    void XChat::fill_recvq()
    {
	if (time(0) - last_recv >= recv_interval) {
	    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
		try { getmsg(i->second); }
		catch (runtime_error e) {
		    throw runtime_error(i->first + ": " + e.what());
		}
	    }

	    last_recv = time(0);
	}
    }

    /*
     * Check if given user is admin
     */
    bool XChat::isadmin(const string &rid, string nick) {
	if (rooms.find(rid) == rooms.end())
	    return false;
	strtolower(nick);

	if (rooms[rid].admin == nick)
	    return true;

	for (vector<string>::iterator i = rooms[rid].admins.begin();
		i != rooms[rid].admins.end(); i++)
	    if (*i == nick)
		return true;

	return false;
    }
}
