#include <cstdio>
#include <cstdlib>
#include <stdbool.h>
#include <iostream>
#include <set>
#include "xchat.h"
#include "smiles.h"
#include "idle.h"
#include "TomiTCP/str.h"

namespace xchat {
    /*
     * Variable definitions
     */
    int idle_interval = 0;
    int recv_interval = 3;
    bool convert_smiles = true;
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
     * Last server broke, count it.
     */
    string XChat::lastsrv_broke()
    {
	servers[lastsrv].last_break = time(0);
	servers[lastsrv].break_count++;

	if (servers[lastsrv].break_count >= tries_to_rest) {
	    EvError *e = new EvError;
	    e->s = "Server " + tomi_ntop(servers[lastsrv].sa) + " is having a rest.";
	    recvq_push(e);
	}

	return tomi_ntop(servers[lastsrv].sa);
    }
	    
    /*
     * Get some good server
     */
    int XChat::makesrv()
    {
	vector<int> good;
	for (vector<server>::iterator i = servers.begin(); i != servers.end(); i++) {
	    if (i->break_count >= tries_to_rest &&
		    (i->last_break + rest_duration) < time(0)) {
		EvError *e = new EvError;
		e->s = "Server " + tomi_ntop(i->sa)
		    + " is no longer having a rest.";
		recvq_push(e);

		i->break_count = 0;
		good.push_back(i - servers.begin());
	    } else if (i->break_count < tries_to_rest &&
		    (i->last_break + nextchance_interval) < time(0)) {
		good.push_back(i - servers.begin());
	    }
	}

	if (good.size())
	    return good[rand() % good.size()];
	else {
	    EvError *e = new EvError;
	    e->s = "All servers considered bad, cleaning.";
	    recvq_push(e);

	    for (vector<server>::iterator i = servers.begin(); i != servers.end(); i++) {
		i->break_count = 0;
		i->last_break = 0;
	    }

	    return rand() % servers.size();
	}
    }

    /*
     * Prepare URL with given full path.
     */
    string XChat::makeurl(const string& path)
    {
	return "http://" + tomi_ntop(servers[lastsrv = makesrv()].sa) + "/" + path;
    }

    /*
     * Prepare URL with given the path which should be appended to uid and sid
     */
    string XChat::makeurl2(const string& path)
    {
	return "http://" + tomi_ntop(servers[lastsrv = makesrv()].sa) + "/~$" +
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
	    send_item &ref = sendq.front(), msg = ref, left = ref;
	    string prepend;

	    /*
	     * Handle whisper with unknown room
	     */
	    if (ref.room.empty()) {
		if (!rooms.size()) {
		    sendq.pop();
		    throw runtime_error("Can't send PRIVMSG's without channel joined");
		}

		/*
		 * Decide if we have to send global msg
		 */
		room *r;
		x_nick *n = findnick(ref.target, &r);
		if (n) {
		    msg.room = r->rid;
		} else {
		    msg.room = rooms.begin()->first;
		    prepend = "/m " + msg.target + " ";
		    msg.target = "~";
		}
	    }

	    /*
	     * Look if we have to split the message
	     */
	    if (u8strlen(ref.msg.c_str()) + u8strlen(prepend.c_str()) > max_msg_length) {
		if (ref.msg.length() && ref.msg[0] == '/') {
		    EvRoomError *ev = new EvRoomError;
		    ev->s = "Message might have been shortened";
		    ev->rid = ref.room;
		    ev->fatal = false;
		    recvq_push(ev);
		} else {
		    if (u8strlen(prepend.c_str()) >= max_msg_length) {
			sendq.pop();
			throw runtime_error("Fuck... this should have never happened!");
		    }

		    int split = u8strlimit(ref.msg.c_str(),
			    max_msg_length - u8strlen(prepend.c_str()));
		    msg.msg.erase(split);
		}
	    }

	    left.msg.erase(0, msg.msg.length());

	    /*
	     * Fix messages splitted before '/' char
	     */
	    if (left.msg.length() && left.msg[0] == '/') {
		left.msg.insert(0, " ");
	    }

	    /*
	     * Post it
	     */
	    if (rooms.find(msg.room) != rooms.end()) {
		putmsg(rooms[msg.room], msg.target, prepend + msg.msg);
	    } else {
		EvRoomError *ev = new EvRoomError;
		ev->s = "Message lost, room is not available";
		ev->rid = msg.room;
		ev->fatal = false;
		recvq_push(ev);
	    }

	    if (left.msg.length())
		ref.msg = left.msg;
	    else
		sendq.pop();
	}

	// f00king idler
	if (idle_interval && sendq.empty())
	    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
		if (time(0) - i->second.last_sent >= idle_interval) {
		    sendq.push(send_item(i->first, nick, genidle()));
		}
	    }
    }

    /*
     * Get new messages if it should be done yet.
     */
    void XChat::fill_recvq()
    {
	if (time(0) - last_recv >= recv_interval) {
	    set<string> srooms;
	    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
		srooms.insert(i->first);
	    }
	    for (set<string>::iterator i = srooms.begin(); i != srooms.end(); i++) {
		try { getmsg(rooms[*i]); }
		catch (runtime_error e) {
		    EvRoomError *f = new EvRoomError;
		    f->s = e.what();
		    f->rid = *i;
		    f->fatal = false;
		    recvq_push(f);
		}
	    }

	    last_recv = time(0);

	    /*
	     * Clear the secondary queue.
	     */
	    old_recvq.clear();
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
    
    bool XChat::ispermadmin(const string &rid, string nick) {
	if (rooms.find(rid) == rooms.end())
	    return false;
	strtolower(nick);

	for (vector<string>::iterator i = rooms[rid].admins.begin();
		i != rooms[rid].admins.end(); i++)
	    if (*i == nick)
		return true;

	return false;
    }
}
