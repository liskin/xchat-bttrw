#include <cstdlib>
#include "xchat.h"
#include "TomiTCP/str.h"

namespace xchat {
    string XChat::makeurl(string path)
    {
	return "http://x" + inttostr(1 + (rand() % servers)) + ".xchat.centrum.cz/" + path;
    }

    string XChat::makeurl2(string path)
    {
	return "http://x" + inttostr(1 + (rand() % servers)) + ".xchat.centrum.cz/~$" +
	    uid + "~" + sid + "/" + path;
    }
}
