#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

namespace xchat {
    /*
     * Join channel and get all needed info about it
     */
    void XChat::join(const string& rid)
    {
	TomiHTTP s;
	string l;
	room r;

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

		    if (strtolower_nr(nick) == strtolower_nr(this->nick))
			mysex = muz;
		}
	    }
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
		
		rooms[rid] = r;
		return;
	    }
	}

	throw runtime_error("Parse error");
    }

    /*
     * Part channel.
     */
    void XChat::part(const string& rid)
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
	     * If we got a redirect to error page, look at the error message.
	     */
	    // window.open("/~$3249019~2a899f0fea802195d52861d6b8e15c1c/modchat?op=fullscreenmessage&key=403&kicking_nick=&text=", '_top');
	}

	/*
	 * Push messages to recvq in reverse.
	 */
	for (vector<string>::reverse_iterator i = tv.rbegin(); i != tv.rend(); i++) {
	    recvq.push(pair<string,string>(r.rid,*i));
	}

	/*
	 * And don't forget to report parse error if we didn't get valid
	 * last_line.
	 */
	if (r.l == -1) {
	    for (vector<string>::iterator i = dbg.begin(); i != dbg.end(); i++)
		cout << *i << endl;
	    throw runtime_error("Parse error");
	}

	last_recv = time(0);
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
