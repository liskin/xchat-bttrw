#include "irc.h"
#include "TomiTCP/str.h"

namespace std {
    /*
     * Parse IRC input line into source, command and parameters
     */
    void parsein(string buf, string& prefix, vector<string>& cmd)
    {
	const char *p = buf.c_str(), *e = buf.c_str() + buf.length();

	if (buf[0] == ':') {
	    p++;
	    char* d = strchr(p,' ');
	    if (d) {
		prefix = string(p,0,(d-p));
		p = d+1;
	    } else {
		prefix = p;
		p = e;
	    }
	}

	while (p < e) {
	    if (p[0] == ':') {
		cmd.push_back(string(p+1));
		p = e;
	    } else {
		char* d = strchr(p,' ');
		if (d) {
		    string s = string(p,0,(d-p));
		    wstrip(s);
		    if (s.length())
			cmd.push_back(s);
		    p = d+1;
		} else {
		    string s = string(p);
		    wstrip(s);
		    if (s.length())
			cmd.push_back(s);
		    p = e;
		}
	    }
	}
    }
}
