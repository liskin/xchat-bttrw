#include <iostream>
#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

namespace xchat {
    void XChat::list(listout_t &listout)
    {
	TomiHTTP s;
	string l;

	int ret = s.GET(makeurl("~guest~/modchat?op=homepage&skin=2"),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while getting list");
	while (s.getline(l)) {
	    unsigned int a, b, c;
	    unsigned int pos = 0;
	    string pat1 = "<option value=\"", pat2 = "\"> ", pat3 = "</option>";

	    while (((a = l.find(pat1, pos)) != string::npos) &&
		    ((b = l.find(pat2, a + pat1.length())) != string::npos) &&
		    ((c = l.find(pat3, b + pat2.length())) != string::npos)) {
		bool fail = 0;
		for (string::iterator i = l.begin() + a + pat1.length();
			i != l.begin() + b; i++)
		    fail |= !isdigit(*i);

		if (!fail) {
		    string rid(l.begin() + a + pat1.length(), l.begin() + b);
		    string name(l.begin() + b + pat2.length(), l.begin() + c);
		    pos = c + pat3.length();

		    striphtmlent(name);
		    listout.push_back(pair<string,string>(rid, recode_to_client(name)));
		} else {
		    pos = a + 1;
		}
	    }
	}
	s.close();
    }
}
