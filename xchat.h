#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>
#include <vector>

namespace xchat {
    using namespace std;

    static const int servers = 5;

    class XChat {
	public:
	    string uid, sid;

	    XChat(const string& user, const string& pass);
	    ~XChat();

	    int join(const string& room, vector<string>& nicklist);
	    int getmsg(const string& room, int lastmsg, vector<string>& msgs);
	    void putmsg(const string& room, const string& msg);

	    static string makeurl(const string& url);
	    string makeurl2(const string& url);

	    static void stripdate(string &m);
	    static void getnick(string &m, string &src, string &target);
    };
}

#endif /* _XCHAT_H_INCLUDED */
