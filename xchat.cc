#include <cstdio>
#include <cstdlib>
#include <stdbool.h>
#include <recodext.h>
#include <iostream>
#include "xchat.h"
#include "smiles.h"
#include "idle.h"
#include "TomiTCP/str.h"

namespace xchat {
    string XChat::makeurl(const string& path)
    {
	return "http://x" + inttostr(1 + (rand() % servers)) + ".xchat.centrum.cz/" + path;
    }

    string XChat::makeurl2(const string& path)
    {
	return "http://x" + inttostr(1 + (rand() % servers)) + ".xchat.centrum.cz/~$" +
	    uid + "~" + sid + "/" + path;
    }

    void XChat::striphtml(string &s)
    {
	unsigned int a, b;

	while (((a = s.find('<')) != string::npos) &&
		((b = string(s, a).find('>')) != string::npos)) {
	    s.erase(s.begin() + a, s.begin() + a + b + 1);
	}
    }

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
    }

    void XChat::striphtmlent(string &m)
    {
	RECODE_OUTER outer = recode_new_outer (false);
	RECODE_REQUEST request = recode_new_request (outer);
	RECODE_TASK task;
	bool success;

	recode_scan_request (request, "html..flat");

	task = recode_new_task (request);
	task->input.buffer = m.c_str();
	task->input.cursor = m.c_str();
	task->input.limit = m.c_str() + m.length();
	task->output.buffer = 0;
	task->output.cursor = 0;
	task->output.limit = 0;
	success = recode_perform_task (task);

	if (task->output.buffer) {
	    *task->output.cursor = 0;
	    m = task->output.buffer;
	    free(task->output.buffer);
	    task->output.buffer = 0;
	}

	recode_delete_task (task);
	recode_delete_request (request);
	recode_delete_outer (outer);
    }

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

    void XChat::do_sendq()
    {
	if (!sendq.empty() && time(0) - last_sent >= send_interval) {
	    pair<string,string> msg = sendq.front(); sendq.pop();
	    putmsg(rooms[msg.first], msg.second);
	}

	// f00king idler
	if (sendq.empty()) {
	    for (rooms_t::iterator i = rooms.begin(); i != rooms.end(); i++) {
		if (time(0) - i->second.last_sent >= idle_interval) {
		    sendq_push(i->first, "/s " + nick + " " +
			    idle_msgs[rand() % idle_msgs_count]);
		}
	    }
	}
    }
}
