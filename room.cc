#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

void parse_updateinfo(string s, string &admin, bool &locked)
{
    s.erase(0, s.find(',') + 1);
    string slocked(s, 0, s.find(','));
    locked = atol(slocked.c_str());
    s.erase(0, slocked.length() + 1);
    s.erase(0, s.find(',') + 1);
    s.erase(0, s.find(',') + 1);
    s.erase(s.find(')'));

    if (!s.length()) {
	admin = "";
	return;
    }

    if (s[0] == '\'' || s[0] == '"') {
	s.erase(s.begin());
	s.erase(s.end() - 1);
    }

    strtolower(s);
    admin = s;
}

namespace xchat {
    /*
     * Join room and get all needed info about it
     */
    void XChat::join(const string& rid)
    {
	TomiHTTP s;
	string l;
	room r;

	r.l = -1;
	r.rid = rid;
	r.last_sent = time(0) - idle_interval + 10;

	int ret = s.GET(makeurl2("modchat?op=mainframeset&skin=2&rid="+rid),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while joining channel");
	s.close();
	
	ret = s.GET(makeurl2("modchat?op=textpageng&skin=2&js=1&rid="+rid),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while joining channel");
	while (s.getline(l)) {
	    if (l.find("<select name=\"target\">") != string::npos) {
		while (s.getline(l)) {
		    if (l.find("</select>") != string::npos)
			break;

		    string nick;
		    stringstream ss(l);
		    getline(ss, nick, '"');
		    getline(ss, nick, '"');

		    bool muz = 0;
		    unsigned int n = l.find(")</option>");
		    if (n != string::npos && n != 0) {
			muz = (l[n-1] == 'M');
		    }

		    if (nick != "~" && nick != "!")
			r.nicklist[strtolower_nr(nick)] = (struct x_nick){nick, muz};
		}
	    }

	    r.nicklist[strtolower_nr(nick)] = (struct x_nick){nick, 2};
	}
	s.close();

	ret = s.GET(makeurl2("modchat?op=roomtopng&skin=2&js=1&rid="+rid),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while joining channel");

	while (s.getline(l)) {
	    string pat = "&inc=1&last_line=";
	    unsigned int pos = l.find(pat);
	    if (pos != string::npos) {
		stringstream ss(string(l,pos+pat.length()));
		string lastline;

		getline(ss,lastline,'"');
		r.l = atol(lastline.c_str());
		continue;
	    }

	    pat = "update_info('";
	    pos = l.find(pat);
	    if (pos != string::npos) {
		parse_updateinfo(string(l,pos+pat.length()), r.admin, r.locked);
	    }
	}

	if (r.l != -1) {
	    rooms[rid] = r;
	    return;
	}

	throw runtime_error("Parse error");
    }

    /*
     * Leave room.
     */
    void XChat::leave(const string& rid)
    {
	TomiHTTP s;

	int ret = s.GET(makeurl2("modchat?op=mainframeset&skin=2&js=1&menuaction=leave"
		    "&leftroom="+rid),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while parting channel");
	
	rooms.erase(rid);
    }

    /*
     * Get new messages.
     */
    void XChat::getmsg(room& r)
    {
	TomiHTTP s;
	int ret;
	try {
	    ret = s.GET(makeurl2("modchat?op=roomtopng&skin=2&js=1&rid=" + r.rid +
			"&inc=1&last_line=" + inttostr(r.l)),0);
	} catch (...) {
	    // we don't want to die on any request timeout
	    return;
	}
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while getting channels msgs");

	vector<string> dbg;

	r.l = -1;
	string l;
	bool expect_apos = false;
	vector<string> tv;
	while (s.getline(l)) {
	    wstrip(l);
	    if (!l.length()) continue;
	    dbg.push_back(l);
	    //cout << l << endl;

	    // look for next last_line number
	    if (r.l == -1) {
		string pat = "&inc=1&last_line=";
		unsigned int pos = l.find(pat);
		if (pos != string::npos) {
		    stringstream ss(string(l,pos+pat.length()));
		    string lastline;

		    getline(ss,lastline,'"');
		    r.l = atol(lastline.c_str());
		}
	    }

	    /*
	     * Parse that scary javascript adding new messages to the list.
	     * (yes, this parsing code is much more scary, so try to fix it
	     * only if you are curios, otherwise, just report it's not
	     * working)
	     */
	    if (expect_apos) {
		expect_apos = false;
		if (l[0] == '\'') {
		    unsigned int pos;
		    if ((pos = string(l,1).find('\'')) != string::npos) {
			tv.push_back(string(l,1,pos));
			if (l[pos+2] == ',') {
			    expect_apos = true;
			}
		    }
		}
	    } else {
		unsigned int pos = 0, pos2, pos3;
		while ((pos3 = string(l,pos).find(".addText(")) != string::npos) {
		    pos += pos3 + 9;
		    if ((pos2 = string(l,pos).find("Array(")) != string::npos) {
			if (l[pos+pos2+6] == '\'') {
			    if ((pos3 = string(l,pos+pos2+7).find('\'')) != string::npos) {
				tv.push_back(string(l,pos+pos2+7,pos3));
				if (l[pos+pos2+7+pos3+1] == ',') {
				    expect_apos = true;
				}
			    }
			}
		    }
		}
	    }

	    /*
	     * Check for current admin and locked status, and eventually emit
	     * appropiate events.
	     */
	    {
		string pat = "update_info('";
		unsigned int pos = l.find(pat);
		if (pos != string::npos) {
		    string admin;
		    bool locked;
		    parse_updateinfo(string(l,pos+pat.length()), admin, locked);
		    if (r.admin != admin) {
			EvRoomAdminChange *e = new EvRoomAdminChange;
			e->s = "Admin change: " + r.admin + " => " + admin;
			e->rid = r.rid;
			e->before = r.admin;
			e->now = admin;
			r.admin = admin;
			recvq_push(e);
		    }

		    if (r.locked != locked) {
			EvRoomLockChange *e = new EvRoomLockChange;
			e->s = "Locked change: " + r.admin + " => " + admin;
			e->rid = r.rid;
			e->before = r.locked;
			e->now = locked;
			r.locked = locked;
			recvq_push(e);
		    }
		}
	    }

	    /*
	     * If we got a redirect to error page, look at the error message.
	     */
	    // window.open("/~$3249019~2a899f0fea802195d52861d6b8e15c1c/modchat?op=fullscreenmessage&key=403&kicking_nick=&text=", '_top');
	}

	/*
	 * Push messages to recvq in reverse.
	 */
	for (vector<string>::reverse_iterator i = tv.rbegin(); i != tv.rend(); i++)
	    recvq_parse_push(*i, r);

	/*
	 * And don't forget to report parse error if we didn't get valid
	 * last_line.
	 */
	if (r.l == -1) {
	    for (vector<string>::iterator i = dbg.begin(); i != dbg.end(); i++)
		cout << *i << endl;
	    EvRoomError *e = new EvRoomError;
	    e->s = r.rid + ": Parse error";
	    e->rid = r.rid;
	    recvq_push(e);
	}
    }

    /*
     * Send a message.
     */
    void XChat::putmsg(room &r, const string& msg)
    {
	TomiHTTP s;
	int ret = s.POST(makeurl2("modchat"),"op=textpage&skin=2&rid="+r.rid+"&aid=0"+
		"&target=~&textarea="+TomiHTTP::URLencode(msg),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while posting msg");
	r.last_sent = last_sent = time(0);
    }
}
