#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <queue>
#include "TomiTCP/net.h"
#include "charset.h"

namespace xchat {
    using namespace std;

    struct x_nick {
	string nick;
	int sex;
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
    const char * const sexhost[] = {
	"girls.xchat.cz",
	"boys.xchat.cz",
	"users.xchat.cz"
    };

    /*
     * Flood protection, refresh rate and idle protection
     * 720 is optimal for flood protection
     */
    const int send_interval = 5, recv_interval = 3;
    extern int idle_interval;

    class XChat {
	public:
	    vector<net::sockaddr_uni> servers;

	    string uid, sid, nick;
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

	    string makeurl(const string& url);
	    string makeurl2(const string& url);

	    static void striphtml(string &s);
	    static void stripdate(string &m);
	    static void getnick(string &m, string &src, string &target);
	    static void unsmilize(string &s);
	    bool isjoin(const string& r, string &m, string &src);
	    bool ispart(const string& r, string &m, string &src, string &host);
	    bool iskick(const string& r, string &m, string &src, string &reason, string &who, string &host);

	    x_nick* findnick(string src, room **r);
	    const char * getsexhost(string src);
    };
}

#endif /* _XCHAT_H_INCLUDED */
