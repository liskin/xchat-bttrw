#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

namespace xchat {
    int XChat::join(const string& room)
    {
	TomiHTTP s;
	int ret = s.GET(makeurl2("modchat?op=mainframeset&rid="+room),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while joining channel");
	s.close();
	
	ret = s.GET(makeurl2("modchat?op=roomframe&rid="+room),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while joining channel");

	string l;
	while (s.getline(l)) {
	    string pat = "modchat?op=roomtop&rid="+room+"&inc=1&last_line=";
	    unsigned int pos = l.find(pat);
	    if (pos != string::npos) {
		stringstream ss(string(l,pos+pat.length()));
		string lastline;

		getline(ss,lastline,'"');
		return atol(lastline.c_str());
	    }
	}

	throw runtime_error("Parse error");
    }

    int XChat::getmsg(const string& room, int lastmsg, vector<string>& msgs)
    {
	TomiHTTP s;
	int ret = s.GET(makeurl2("modchat?op=roomtop&rid="+room+"&inc=1&last_line="+
		    inttostr(lastmsg)),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while getting channels msgs");

	lastmsg = -1;
	string l;
	bool expect_apos = false;
	vector<string> tv;
	while (s.getline(l)) {
	    wstrip(l);
	    if (!l.length()) continue;

	    if (lastmsg == -1) {
		string pat = "modchat?op=roomtop&rid="+room+"&inc=1&last_line=";
		unsigned int pos = l.find(pat);
		if (pos != string::npos) {
		    stringstream ss(string(l,pos+pat.length()));
		    string lastline;

		    getline(ss,lastline,'"');
		    lastmsg = atol(lastline.c_str());
		}
	    }

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
	}

	for (vector<string>::reverse_iterator i = tv.rbegin(); i != tv.rend(); i++) {
	    msgs.push_back(*i);
	}

	if (lastmsg == -1) {
	    throw runtime_error("Parse error");
	}

	return lastmsg;
    }
}
