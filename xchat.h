#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <ctime>

namespace xchat {
    using namespace std;

    static const int servers = 5;

    struct x_nick {
	string nick;
	bool sex;
    };

    typedef map<string,x_nick> nicklist_t;
    struct room {
	int l;
	nicklist_t nicklist;
	time_t last_sent;
    };
    typedef map<string,room> rooms_t;

    class XChat {
	public:
	    string uid, sid;

	    XChat(const string& user, const string& pass);
	    ~XChat();

	    room join(const string& rid);
	    void part(const string& rid);
	    int getmsg(const string& rid, int lastmsg, vector<string>& msgs);
	    void putmsg(const string& rid, const string& msg);

	    static string makeurl(const string& url);
	    string makeurl2(const string& url);

	    static void striphtml(string &s);
	    static void stripdate(string &m);
	    static void getnick(string &m, string &src, string &target);
	    static void striphtmlent(string &m);
	    static void unsmilize(string &s);
	    static bool isjoin(string &m, rooms_t &rooms, string &src, const string& room);
	    static bool ispart(string &m, rooms_t &rooms, string &src);
	    static bool iskick(string &m, rooms_t &rooms, string &src,
		    string &reason, string &who);
    };
}

#endif /* _XCHAT_H_INCLUDED */
