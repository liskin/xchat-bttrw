#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

/**
 * Parse the arguments to JavaScript function update_info.
 * \param s Arguments as one string.
 * \param admin Admin is stored here.
 * \param locked Locked state is stored here.
 */
static void parse_updateinfo(string s, string &admin, bool &locked)
{
    s.erase(0, s.find(',') + 1);
    string slocked(s, 0, s.find(','));
    locked = atoi(slocked.c_str());
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
    if (admin == "-----")
	admin = "";
}

/**
 * Check, if the 'chvilku strpeni prosim' msg is present in the input.
 * \param l Input line.
 */
static bool tryagainplease(const string &l)
{
    static string pat = "chvilku strpení prosím</body></html>";
    if (l.find(pat) != string::npos)
	return true;
    return false;
}

namespace xchat {
    /**
     * Join room and get all needed info about it. The room is then saved into
     * the #rooms array.
     * \param rid Room id.
     */
    void XChat::join(const string& rid)
    {
	TomiHTTP s;
	string l;
	room r;

	r.l = -1;
	r.rid = rid;
	r.last_sent = 0;
	r.last_roominfo = time(0);

	int ret, retries;

	retries = servers.size();
retry1:
	try {
	    ret = s.GET(makeurl2("modchat?op=mainframeset&skin=2&rid=" + rid),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while joining channel");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry1;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}
	while (s.getline(l)) {
	    static string pat1 = "<h3 class=\"hdrsuccess\">", pat2 = "</h3>";
	    unsigned int a, b;
	    if ((a = l.find(pat1)) != string::npos &&
		    (b = l.find(pat2, a + pat1.length())) != string::npos) {
		string err(l, a + pat1.length(), b - a - pat1.length());
		striphtml(err);
		striphtmlent(err);
		unsmilize(err);
		throw runtime_error(err);
	    }

	    if (tryagainplease(l)) {
		if (retries--) {
		    lastsrv_broke();
		    goto retry1;
		} else
		    throw runtime_error("Chvilku strpeni prosim - " + lastsrv_broke());
	    }
	}
	s.close();
	
	retries = servers.size();
retry2:
	try {
	    ret = s.GET(makeurl2("modchat?op=roomtopng&skin=2&js=1&rid=" + rid),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while joining channel");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry2;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}
	bool lastline = false, updateinfo = false;
	while (s.getline(l)) {
	    static string pat1 = "&inc=1&last_line=";
	    unsigned int a, b;
	    if ((a = l.find(pat1)) != string::npos &&
		    (b = l.find('"', a + pat1.length())) != string::npos) {
		lastline = true;
		r.l = atoi(string(l, a + pat1.length(), b - a - pat1.length()).c_str());
		continue;
	    }

	    static string pat2 = "update_info('";
	    if ((a = l.find(pat2)) != string::npos) {
		updateinfo = true;
		parse_updateinfo(string(l, a + pat2.length()), r.admin, r.locked);
	    }
	}
	if (!lastline || !updateinfo) {
	    if (retries--) {
		lastsrv_broke();
		goto retry2;
	    } else
		throw runtime_error("No initial room params - " + lastsrv_broke());
	}
	s.close();

	// didn't get the last_line
	if (r.l == -1)
	    throw runtime_error("Parse error");

	retries = servers.size();
retry3:
	try {
	    ret = s.GET(makeurl2("modchat?op=textpageng&skin=2&js=1&rid=" + rid),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while joining channel");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry3;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}
	while (s.getline(l)) {
	    if (l.find("<select name=\"target\">") != string::npos) {
		while (s.getline(l)) {
		    if (l.find("</select>") != string::npos)
			break;

		    unsigned int a, b = 0;
		    static string pat1 = "<option ", pat2 = "</option>";
		    while ((a = l.find(pat1, b)) != string::npos &&
			    (b = l.find(pat2, a)) != string::npos) {
			string o(l, a, b - a + pat2.length());

			unsigned int pos, pos2;
			static string pat = "value=\"";
			o.erase(0, o.find(pat));
			o.erase(0, pat.length());
			string nick(o, 0, o.find('"'));
			o.erase(0, nick.length() + 1);

			bool muz = 0;
			if ((pos = o.find(")</option>")) != string::npos && pos != 0 &&
				(pos2 = o.find_last_of('(', pos)) != string::npos) {
			    string sex(o, pos2 + 1, pos - pos2 - 1);
			    if (sex == "M")
				muz = 1;
			    else if (sex == "Ž")
				muz = 0;
			    else
				continue;
			}

			if (nick != "~" && nick != "!")
			    r.nicklist[strtolower_nr(nick)] = (struct x_nick){nick, muz};
		    }
		}
	    }
	}
	s.close();
	r.nicklist[strtolower_nr(me.nick)] = me;

	try { getroominfo(r); }
	catch (runtime_error e) {
	    EvRoomError *f = new EvRoomError;
	    f->s = e.what();
	    f->rid = r.rid;
	    f->fatal = false;
	    recvq_push(f);
	}

	// insert it
	rooms[rid] = r;
    }

    /**
     * Leave room.
     * \param rid Room id.
     */
    void XChat::leave(string rid)
    {
	rooms.erase(rid);
	
	TomiHTTP s;

	int retries = servers.size();
retry:
	try {
	    int ret = s.GET(makeurl2("modchat?op=mainframeset&skin=2&js=1&menuaction=leave"
			"&leftroom=" + rid),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while parting channel");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	string l;
	bool bad = false;
	if (s.getline(l)) {
	    string pat = "<html><head><meta http-equiv=\"Refresh\"";
	    if (!l.compare(0, pat.length(), pat))
		bad = true;
	}
	if (bad) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error("Unsuccessful leave - " + lastsrv_broke());
	}
    }

    /**
     * Get room info.
     * \param r Reference to room structure which we want to fill with room
     * info.
     */
    void XChat::getroominfo(room& r)
    {
	TomiHTTP s;
	string l;

	int ret, retries;

	retries = servers.size();
retry:
	try {
	    ret = s.GET(makeurl2("modchat?op=roominfo&skin=2&rid=" + r.rid),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while joining channel");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	while (s.getline(l)) {
	    chomp(l);

	    static string pat1 = "název místnosti:</td>";
	    unsigned int pos;
	    if ((pos = l.find(pat1)) != string::npos) {
		string st(l, pos + pat1.length());
		striphtml(st);
		striphtmlent(st);
		wstrip(st);
		r.name = recode_to_client(st);
		continue;
	    }
	    
	    static string pat2 = "popis místnosti:</td>";
	    if ((pos = l.find(pat2)) != string::npos) {
		string st(l, pos + pat2.length());
		striphtml(st);
		striphtmlent(st);
		wstrip(st);
		unsmilize(st);
		r.desc = recode_to_client(st);
		continue;
	    }
	    
	    static string pat3 = "stálý správce:</td>", pat4 = "</td>";
	    if ((pos = l.find(pat3)) != string::npos) {
		s.getline(l);
		if ((pos = l.find(pat4)) != string::npos)
		    l.erase(pos);
		striphtml(l);
		striphtmlent(l);
		wstrip(l);
		stringstream ss(l);
		string admin;
		r.admins.clear();
		while (ss >> admin)
		    r.admins.insert(strtolower_nr(admin));
		continue;
	    }
	}

	if (!r.name.length() && !r.desc.length() && !r.admins.size()) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error("No roominfo - " + lastsrv_broke());
	}
    }

    /**
     * Get new messages from room.
     * \param r Reference to the room.
     */
    void XChat::getmsg(room& r)
    {
	TomiHTTP s;
	int ret;
	try {
	    ret = s.GET(makeurl2("modchat?op=roomtopng&skin=2&js=1&rid=" + r.rid +
			"&inc=1&last_line=" + ((r.l>=0)?inttostr(r.l):"")),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting channels msgs");
	} catch (runtime_error e) {
	    throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	vector<string> dbg;
	string kicker, kickmsg;
	EvRoomAdminChange *erac = 0;
	EvRoomLockChange *erlc = 0;

	int old_l = r.l;
	r.l = -1;
	string l;
	bool expect_apos = false;
	vector<string> tv;
	while (s.getline(l)) {
	    wstrip(l);
	    if (!l.length()) continue;
	    dbg.push_back(l);

	    // look for next last_line number
	    if (r.l == -1) {
		static string pat1 = "&inc=1&last_line=";
		unsigned int a, b;
		if ((a = l.find(pat1)) != string::npos &&
			(b = l.find('"', a + pat1.length())) != string::npos) {
		    r.l = atoi(string(l, a + pat1.length(), b - a - pat1.length()).c_str());
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
		    unsigned int a, b = 1;
		    while ((a = l.find('\'', b)) != string::npos) {
			if (l[a - 1] == '\\') {
			    b = a + 1;
			    continue;
			}

			tv.push_back(string(l, 1, a - 1));
			if (l[a + 1] == ',')
			    expect_apos = true;

			break;
		    }
		}
	    } else {
		unsigned int pos1, pos2, a, b;
		static string pat1 = ".addText(", pat2 = "Array('";

		if ((pos1 = l.find(pat1)) != string::npos) {
		    if ((pos2 = l.find(pat2, pos1 + pat1.length())) != string::npos) {
			b = pos2 + pat2.length();
			while ((a = l.find('\'', b)) != string::npos) {
			    if (l[a - 1] == '\\') {
				b = a + 1;
				continue;
			    }

			    tv.push_back(string(l, pos2 + pat2.length(),
					a - pos2 - pat2.length()));
			    if (l[a + 1] == ',')
				expect_apos = true;

			    break;
			}
		    }
		}
	    }

	    /*
	     * Check for current admin and locked status
	     */
	    {
		static string pat = "update_info('";
		unsigned int pos;
		if ((pos = l.find(pat)) != string::npos) {
		    string admin;
		    bool locked;
		    parse_updateinfo(string(l,pos+pat.length()), admin, locked);
		    if (r.admin != admin) {
			if (erac)
			    delete erac;
			erac = new EvRoomAdminChange;
			erac->rid = r.rid;
			erac->before = r.admin;
			erac->now = admin;
			r.admin = admin;
		    }

		    if (r.locked != locked) {
			if (erlc)
			    delete erlc;
			erlc = new EvRoomLockChange;
			erlc->rid = r.rid;
			erlc->before = r.locked;
			erlc->now = locked;
			r.locked = locked;
		    }
		}
	    }

	    /*
	     * If we got a redirect to error page, look at the error message.
	     */
	    static string pat1 = "top.location='", pat2 = "modchat?op=fullscreenmessage";
	    unsigned int a, b, c;
	    if ((a = l.find(pat1)) != string::npos &&
		    (b = l.find(pat2, a)) != string::npos &&
		    (c = l.find('\'', b)) != string::npos) {
		string url(l, b, c - b);

		static string pat3 = "&kicking_nick=", pat4 = "&text=";
		unsigned int pos;
		if ((pos = url.find(pat3)) != string::npos) {
		    kicker = string(url, pos + pat3.length(),
			    url.find('&', pos + pat3.length()) - pos - pat3.length());
		    kicker = TomiHTTP::URLdecode(kicker);
		    wstrip(kicker);
		}

		if ((pos = url.find(pat4)) != string::npos) {
		    kickmsg = string(url, pos + pat4.length(),
			    url.find('&', pos + pat4.length()) - pos - pat4.length());
		    kickmsg = TomiHTTP::URLdecode(kickmsg);
		    wstrip(kickmsg);
		}

		if (!kicker.length() && !kickmsg.length()) {
		    TomiHTTP c; string m;
		    try {
			ret = c.GET(makeurl2(url),0);
			if (ret != 200)
			    throw 0;
			while (c.getline(m)) {
			    static string pat1 = "<h3 class=\"hdrsuccess\">", pat2 = "</h3>";
			    unsigned int a, b;
			    if ((a = m.find(pat1)) != string::npos &&
				    (b = m.find(pat2, a + pat1.length())) != string::npos) {
				kickmsg = string(m, a + pat1.length(), b - a - pat1.length());
				striphtml(kickmsg);
				striphtmlent(kickmsg);
				unsmilize(kickmsg);
				break;
			    }
			}
		    } catch (...) {
			kickmsg = "Error, but I could not fetch error message - timeout";
		    }
		}
	    }

	    if (tryagainplease(l)) {
		r.l = old_l;
		throw runtime_error("Chvilku strpeni prosim - " + lastsrv_broke());
	    }
	}

	/*
	 * Push messages to recvq in reverse.
	 */
	for (vector<string>::reverse_iterator i = tv.rbegin(); i != tv.rend(); i++)
	    if (i->length())
		recvq_parse_push(*i, r);

	/*
	 * Emit Admin/LockChange
	 */
	if (erac)
	    recvq_push(erac);
	if (erlc)
	    recvq_push(erlc);

	/*
	 * Look if we should emit a kick/error message
	 */
	if (kicker.length()) {
	    x_nick *n;
	    EvRoomKick *e = new EvRoomKick;
	    e->s = kicker + " kicked you because: " + recode_to_client(kickmsg);
	    e->rid = r.rid;
	    e->src = (struct x_nick){ kicker, (n = findnick(kicker, 0))?n->sex:2 };
	    e->target = me;
	    e->reason = recode_to_client(kickmsg);
	    recvq_push(e);
	    rooms.erase(e->rid);
	} else if (kickmsg.length()) {
	    EvRoomError *e = new EvRoomError;
	    e->s = recode_to_client(kickmsg);
	    e->rid = r.rid;
	    e->fatal = true;
	    recvq_push(e);
	    rooms.erase(e->rid);
	} else if (r.l == -1) {
	    for (vector<string>::iterator i = dbg.begin(); i != dbg.end(); i++)
		cout << *i << endl;
	    EvRoomError *e = new EvRoomError;
	    e->s = "Parse error";
	    e->rid = r.rid;
	    e->fatal = true;
	    recvq_push(e);
	    rooms.erase(e->rid);
	}
    }

    /**
     * Send a message.
     * \param r Reference to the room.
     * \param target The message target or "~".
     * \param msg The message.
     * \return True if the message was posted, false if xchat rejected it. In
     * case of network/HTTP/cluster error, exception is thrown.
     */
    bool XChat::putmsg(room &r, const string& target, const string& msg)
    {
	TomiHTTP s;
	try {
	    int ret = s.POST(makeurl2("modchat"),"op=textpage&skin=2&rid=" + r.rid +
		    "&aid=0" + "&target=" + TomiHTTP::URLencode(target) +
		    "&textarea=" + TomiHTTP::URLencode(msg), 0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while posting msg");
	} catch (runtime_error e) {
	    throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	bool posted = true;

	string l;
	while (s.getline(l)) {
	    /*
	     * Check for 'chvilku strpeni prosim'-like error.
	     * If we're alone, we can't check :/
	     */
	    static string pat = ">zaslat všem (0)";
	    if (l.find(pat) != string::npos)
		if (r.nicklist.size() > 1)
		    throw runtime_error("Message was likely not posted, trying again - "
			    + lastsrv_broke());

	    /*
	     * Check, if the message was posted by looking if they have left
	     * something in textarea.
	     */
	    static string pat2 = " name=\"textarea\" value=\"";
	    unsigned int a;
	    if ((a = l.find(pat2)) != string::npos && l[a + pat2.length()] != '\"')
		posted = false;
	}

	/*
	 * Update last_sent, if
	 *  - it has a nonzero length
	 *  and
	 *  - it does not begin with "/" and it does not begin with a
	 *    nonprintable character
	 */
	last_sent = time(0);
	if (msg.length() && msg[0] != '/' && isprint(msg[0])) {
	    r.last_sent = last_sent;
	}

	return !posted;
    }

    /**
     * Set room description.
     * \param rid Room id.
     * \param desc The description to be set.
     */
    void XChat::setdesc(const string& rid, const string& desc)
    {
	TomiHTTP s;

	int retries = servers.size();
retry:
	try {
	    int ret = s.POST(makeurl2("modchat"),"op=rightadmin&skin=2&rid=" + rid +
		    "&desc=" + TomiHTTP::URLencode(recode_from_client(desc)), 0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while setting room desc");
	} catch (runtime_error e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}
	
	/*
	 * TODO: detect it somehow, cos it's probably not tryagainplease...
	 */
	string l;
	while (s.getline(l)) {
	    if (tryagainplease(l)) {
		if (retries--) {
		    lastsrv_broke();
		    goto retry;
		} else
		    throw runtime_error("Chvilku strpeni prosim - " + lastsrv_broke());
	    }
	}
    }
}
