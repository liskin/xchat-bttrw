#include <cstdio>
#include <cstdlib>
#include <stdbool.h>
#include <recodext.h>
#include <iostream>
#include "xchat.h"
#include "smiles.h"
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
		    s.insert(a, smiles[smile]);
		    pos = a + strlen(smiles[smile]);
		} else {
		    pos = a + 1;
		}
	    } else {
	    }
	}
    }

    bool XChat::isjoin(string &m, rooms_t &rooms, string &src, const string& room)
    {
	unsigned int a,b;
	if ((a = m.find("Uzivatel")) != string::npos &&
		(b = m.find("vstoupil")) != string::npos) {
	    if (m.find("Uzivatelka") != string::npos) {
		src = string(m, a + sizeof("Uzivatelka ") - 1, b - a - sizeof("Uzivatelka ") + 1);
		wstrip(src);
		rooms[room].nicklist[strtolower_nr(src)] = (struct x_nick){src, 0};
	    } else {
		src = string(m, a + sizeof("Uzivatel ") - 1, b - a - sizeof("Uzivatel ") + 1);
		wstrip(src);
		rooms[room].nicklist[strtolower_nr(src)] = (struct x_nick){src, 1};
	    }
	    return 1;
	}

	return 0;
    }

    bool XChat::ispart(string &m, rooms_t &rooms, string &src)
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
	    return 1;
	}

	return 0;
    }

    bool XChat::iskick(string &m, rooms_t &rooms, string &src, string &reason, string &who)
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
	    return 1;
	}

	return 0;
    }
}
