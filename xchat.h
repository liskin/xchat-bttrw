#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <queue>

namespace xchat {
    using namespace std;

    static const int servers = 5;

    struct x_nick {
	string nick;
	bool sex;
    };

    typedef map<string,x_nick> nicklist_t;
    struct room {
	string rid;
	int l;
	nicklist_t nicklist;
	time_t last_sent;
    };
    typedef map<string,room> rooms_t;

    const char * const me = "xchat.cz";
    const char * const userhost = "users.xchat.cz";
    const char * const sexhost[] = {
	"girls.xchat.cz",
	"boys.xchat.cz"
    };

    const int send_interval = 5, recv_interval = 3, idle_interval = 840;

    class XChat {
	public:
	    string uid, sid, nick;
	    int mysex;
	    rooms_t rooms;

	    time_t last_sent, last_recv;
	    queue<pair<string,string> > sendq; // chan => msg
	    queue<pair<string,string> > recvq; // chan => msg

	    void sendq_push(const string& a, const string& b) {
		sendq.push(pair<string,string>(a,b));
	    }
	    void do_sendq();
	    bool should_recv() {
		return time(0) - last_recv >= recv_interval;
	    }

	    XChat(const string& user, const string& pass);
	    ~XChat();

	    void join(const string& rid);
	    void part(const string& rid);
	    void getmsg(room& r);
	    void putmsg(room& r, const string& msg);

	    static string makeurl(const string& url);
	    string makeurl2(const string& url);

	    static void striphtml(string &s);
	    static void stripdate(string &m);
	    static void getnick(string &m, string &src, string &target);
	    static void striphtmlent(string &m);
	    static void unsmilize(string &s);
	    bool isjoin(const string& r, string &m, string &src);
	    bool ispart(const string& r, string &m, string &src, string &host);
	    bool iskick(const string& r, string &m, string &src, string &reason, string &who, string &host);

	    x_nick* findnick(string nick, room **r);
	    const char * getsexhost(string src);
    };
}

#endif /* _XCHAT_H_INCLUDED */
