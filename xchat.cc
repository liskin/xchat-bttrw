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
     * Strip HTML tags
     */
    void XChat::striphtml(string &s)
    {
	unsigned int a, b;

	while (((a = s.find('<')) != string::npos) &&
		((b = string(s, a).find('>')) != string::npos)) {
	    s.erase(s.begin() + a, s.begin() + a + b + 1);
	}
    }

    /*
     * Strip date, if it is there.
     */
    void XChat::stripdate(string &m)
    {
	string n = m, d;

	n.erase(0, n.find_first_not_of(" "));
	d = string(n, 0, n.find_first_of(" "));
	n.erase(0, n.find_first_of(" "));
	n.erase(0, n.find_first_not_of(" "));

	d.erase(0, d.find_first_of(":"));
	d.erase(d.find_last_of(":") + 1);
	if (d.length() > 1 && d[0] == ':' && d[d.length()-1] == ':') {
	    m = n;
	}
    }

    /*
     * Get nick (source) and, if given, target nick
     */
    void XChat::getnick(string &m, string &src, string &target)
    {
	string t = string(m, 0, m.find(": "));
	if (t.length() == m.length())
	    return;
	m.erase(0, t.length() + 2);
	if (m.length() && m[0] == ' ')
	    m.erase(m.begin());

	t.erase(0, t.find_first_not_of(" "));
	src = string(t, 0, t.find("->"));
	if (src.length() != t.length())
	    target = string(t, src.length() + 2);

	// strip [room]
	unsigned int a;
	if (src[0] == '[' && ((a = src.find(']')) != string::npos)) {
	    src.erase(0, a + 1);
	}
    }

    /*
     * Init recode library.
     */
    recode_t recode;
    void init_recode()
    {
	recode.outer = recode_new_outer (false);
       	recode.request = recode_new_request (recode.outer);
	recode_scan_request (recode.request, "html..flat");
    }

    /*
     * Free recode library.
     */
    void exit_recode()
    {
	recode_delete_request (recode.request);
	recode_delete_outer (recode.outer);
    }

    /*
     * Convert HTML entities to plain 7-bit ascii
     */
    void XChat::striphtmlent(string &m)
    {
	char *ret = recode_string(recode.request, m.c_str());
	if (ret) {
	    m = ret;
	    free(ret);
	}
    }

    /*
     * Convert xchat smilies to human readable ones
     */
    void XChat::unsmilize(string &s)
    {
	unsigned int a, b;
	unsigned int pos = 0;

	while (((a = s.find('*', pos)) != string::npos) &&
		((b = s.find('*', a + 1)) != string::npos)) {
	    bool fail = 0;
	    for (string::iterator i = s.begin() + a + 1; i != s.begin() + b; i++)
		fail |= !isdigit(*i);

	    if (!fail) {
		int smile = atol(string(s.begin() + a + 1, s.begin() + b).c_str());
		if (smile < smiles_count && smiles[smile]) {
		    s.erase(s.begin() + a, s.begin() + b + 1);
		    s.insert(a, string("\002") + smiles[smile] + "\002");

		    int add = 2;
		    if (s[a + strlen(smiles[smile]) + 2] != ' ') {
			s.insert(a + strlen(smiles[smile]) + 2, " ");
			add++;
		    }
		    if (a > 0 && s[a - 1] != ' ') {
			s.insert(a, " ");
			add++;
		    }

		    pos = a + strlen(smiles[smile]) + add;
		} else {
		    pos = a + 1;
		}
	    } else {
		pos = a + 1;
	    }
	}
    }

    /*
     * Check for user joining a room
     */
    bool XChat::isjoin(const string& r, string &m, string &src)
    {
	unsigned int a,b;
	if ((a = m.find("Uzivatel")) != string::npos &&
		(b = m.find("vstoupil")) != string::npos) {
	    if (m.find("Uzivatelka") != string::npos) {
		src = string(m, a + sizeof("Uzivatelka ") - 1, b - a - sizeof("Uzivatelka ") + 1);
		wstrip(src);
		rooms[r].nicklist[strtolower_nr(src)] = (struct x_nick){src, 0};
	    } else {
		src = string(m, a + sizeof("Uzivatel ") - 1, b - a - sizeof("Uzivatel ") + 1);
		wstrip(src);
		rooms[r].nicklist[strtolower_nr(src)] = (struct x_nick){src, 1};
	    }
	    return 1;
	}

	return 0;
    }

    /*
     * Check for user leaving a room
     */
    bool XChat::ispart(const string& r, string &m, string &src, string &host)
    {
	unsigned int a,b;
	if ((a = m.find("Uzivatel")) != string::npos &&
		(b = m.find("opustil")) != string::npos) {
	    if (m.find("Uzivatelka") != string::npos) {
		src = string(m, a + sizeof("Uzivatelka ") - 1, b - a - sizeof("Uzivatelka ") + 1);
		wstrip(src);
	    } else {
		src = string(m, a + sizeof("Uzivatel ") - 1, b - a - sizeof("Uzivatel ") + 1);
		wstrip(src);
	    }

	    host = getsexhost(src);
	    rooms[r].nicklist.erase(strtolower_nr(src));
	    return 1;
	}

	return 0;
    }

    /*
     * Check for user being kicked from room
     */
    bool XChat::iskick(const string& r, string &m, string &src, string &reason, string &who, string &host)
    {
	unsigned int a,b;
	if ((a = m.find("Uzivatel")) != string::npos &&
		(((b = m.find("byl  vyhozen")) != string::npos) ||
		(b = m.find("byla vyhozena")) != string::npos)) {
	    if (m.find("Uzivatelka") != string::npos) {
		src = string(m, a + sizeof("Uzivatelka ") - 1, b - a - sizeof("Uzivatelka ") + 1);
		wstrip(src);
	    } else {
		src = string(m, a + sizeof("Uzivatel ") - 1, b - a - sizeof("Uzivatel ") + 1);
		wstrip(src);
	    }

	    if ((a = m.find("spravcem")) != string::npos &&
		    (b = m.find("z mistnosti")) != string::npos) {
		who = string(m, a + sizeof("spravcem ") - 1, b - a - sizeof("spravcem ") + 1);
		wstrip(who);
	    }
	    
	    if ((a = m.find("administratorem")) != string::npos &&
		    (b = m.find("z mistnosti")) != string::npos) {
		who = string(m, a + sizeof("administratorem ") - 1,
			b - a - sizeof("administratorem ") + 1);
		wstrip(who);
	    }

	    if ((a = m.find_last_of("(")) != string::npos &&
		    (b = m.find_last_of(")")) != string::npos) {
		reason = string(m, a + 1, b - a - 1);
	    }

	    host = getsexhost(src);
	    rooms[r].nicklist.erase(strtolower_nr(src));
	    return 1;
	}

	return 0;
    }

    /*
     * Find a nick structure in rooms we are
     */
    x_nick* XChat::findnick(string nick, room **r)
    {
	strtolower(nick);
	for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
	    nicklist_t::iterator n = i->second.nicklist.find(nick);
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
	if (src == strtolower_nr(nick))
	    return sexhost[mysex];

	for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++)
	    if (i->second.nicklist.find(src) != i->second.nicklist.end())
		return sexhost[i->second.nicklist[src].sex];

	return userhost;
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
		    sendq_push(i->first, "/s " + nick + " " +
			    idle_msgs[rand() % idle_msgs_count]);
		}
	    }
    }
}
