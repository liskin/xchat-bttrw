#include <cstdlib>
#include <sstream>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"
using namespace net;

namespace xchat {
    XChat::XChat(const string& user, const string& pass)
    {
	TomiHTTP s;
	int ret = s.POST("http://xchat.centrum.cz/index.php",
		"js=1&name="+TomiHTTP::URLencode(user)+"&pass="+
		TomiHTTP::URLencode(pass),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while logging in");

	string l;
	s.getline(l);

	unsigned int pos = l.find("xchat.centrum.cz/~$");
	if (pos == string::npos)
	    throw runtime_error("Parse error while logging in");

	stringstream ss(string(l,pos+19));
	getline(ss,uid,'~');
	getline(ss,sid,'/');

	if (!uid.length() || !sid.length())
	    throw runtime_error("Parse error while logging in");
    }

    XChat::~XChat()
    {
	TomiHTTP s;
	int ret = s.GET(makeurl("~~logout/~$" + uid + "~" + sid + "/"),0);
	if (ret != 302)
	    throw runtime_error("Not HTTP 302 Ok while logging off");
    }
}
