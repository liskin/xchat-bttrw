#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

namespace xchat {
    int XChat::join(const string& room, const string& cat)
    {
	TomiHTTP s;
	int ret = s.GET(makeurl2("modchat?op=mainframeset&rid="+room+"&cid="+cat),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while joining channel");
	s.close();
	
	ret = s.GET(makeurl2("modchat?op=roomframe&rid="+room+"&cid="+cat),0);
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

    int XChat::getmsg(const string& room, int lastmsg, vector<msg_t>& msgs)
    {
	TomiHTTP s;
	int ret = s.GET(makeurl2("modchat?op=roomtop&rid="+room+"&inc=1&last_line="+
		    inttostr(lastmsg)),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while getting channels msgs");

	lastmsg = -1;
	string l;
	while (s.getline(l)) {
	    cout << l << endl;
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
	}

	if (lastmsg == -1) {
	    throw runtime_error("Parse error");
	}

	return lastmsg;
    }
}
