#include <iostream>
#include <cstdlib>
#include "xchat.h"
#include "TomiTCP/http.h"

using namespace net;

namespace xchat {
    /*
     * Check, if a nick is online.
     * This uses a length of reply from scripts/online_img.php, which may
     * change, so count with this possibility.
     */
    bool XChat::ison(const string& nick)
    {
	TomiHTTP s;
	char buffer[1024];

	int ret = s.GET(makeurl("scripts/online_img.php?nick=" + 
		    TomiHTTP::URLencode(nick)),0);
	if (ret != 200)
	    throw runtime_error("Not HTTP 200 Ok while getting online status");
	int sz = fread(buffer, 1, 1024, s);
	s.close();

	if (sz == 255)
	    return true;
	if (sz == 248)
	    return false;

	throw runtime_error("Unexpected online status reply, please contact developer team!");
    }
}
