#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>

namespace xchat {
    using namespace std;

    static const int servers = 5;

    class XChat {
	public:
	    string uid, sid;

	    XChat(const string& user, const string& pass);
	    ~XChat();

	    static string makeurl(string url);
	    string makeurl2(string url);
    };
}

#endif /* _XCHAT_H_INCLUDED */
