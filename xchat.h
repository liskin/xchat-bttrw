#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <queue>
#include <memory>
#include "TomiTCP/net.h"
#include "TomiTCP/str.h"
#include "charset.h"

namespace xchat {
    using namespace std;

    void xchat_init();

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
	string admin;
	bool locked;
	string name, desc;
	vector<string> admins;
    };
    typedef map<string,room> rooms_t;
    typedef vector<pair<string, string> > listout_t;
}

#include "events.h"

namespace xchat {
    /*
     * Flood protection, refresh rate and idle protection
     * 720 is optimal for flood protection
     */
    const int send_interval = 5, recv_interval = 3;
    extern int idle_interval;

    /*
     * Recoding
     */
    extern string client_charset;

    class recv_item {
	public:
	    recv_item(const recv_item &a) : e(a.e) { }
	    recv_item(Event *f) : e(f) { }
	    mutable auto_ptr<Event> e;
    };

    class XChat {
	public:
	    vector<net::sockaddr_uni> servers;

	    string uid, sid, nick;
	    rooms_t rooms;

	    time_t last_sent, last_recv;
	    queue<pair<string,string> > sendq; // chan => msg
	    queue<recv_item> recvq;

	    void sendq_push(const string& a, const string& b);
	    void do_sendq();
	    void fill_recvq();
	    void recvq_push(Event *e);
	    void recvq_parse_push(string m, room& r);
	    Event * recvq_pop();
	    string recode_to_client(const string& s);

	    XChat(const string& user, const string& pass);
	    ~XChat();

	    void join(const string& rid);
	    void leave(const string& rid);
	    void getmsg(room& r);
	    void putmsg(room& r, const string& msg);

	    string makeurl(const string& url);
	    string makeurl2(const string& url);

	    static void striphtml(string &s);
	    static void stripdate(string &m);
	    static void getnick(string &m, string &src, string &target);
	    static void unsmilize(string &s);
	    bool isjoin(room& r, string &m, string &src, int &sex);
	    bool isleave(room& r, string &m, string &src, int &sex);
	    bool iskick(room& r, string &m, string &src, string &reason, string &who, int &sex);
	    bool isadvert(string &m, string &link);

	    void msg(const string &room, const string &msg);
	    void whisper(const string &room, const string &target, const string &msg);
	    void whisper(const string &target, const string &msg);
	    void kick(const string &room, const string &user, const string &reason);

	    x_nick* findnick(string src, room **r);
	    bool isadmin(const string &rid, string nick);

	    void list(listout_t &listout);
    };

    inline void XChat::sendq_push(const string& a, const string& b) {
	sendq.push(pair<string,string>(a,
		    (client_charset.length())?recode(b, client_charset, "UTF-8"):b));
    }

    inline void XChat::recvq_push(Event *e) {
	recvq.push(recv_item(e));
    }

    inline Event * XChat::recvq_pop() {
	auto_ptr<Event> e = recvq.front().e; recvq.pop();
	return e.release();
    }
    
    inline void XChat::msg(const string &room, const string &msg) {
	sendq_push(room, msg);
    }

    inline void XChat::whisper(const string &room, const string &target, const string &msg) {
	sendq_push(room, "/m " + target + " " + msg);
    }
    
    inline void XChat::whisper(const string &target, const string &msg) {
	if (rooms.size()) {
	    // decide if we have to send global msg
	    room *r;
	    x_nick *n = findnick(target, &r);
	    if (n)
		sendq_push(r->rid, "/s " + target + " " + msg);
	    else
		sendq_push(rooms.begin()->first, "/m " + target + " " + msg);
	} else {
	    throw runtime_error("Can't send PRIVMSG's without channel joined");
	}
    }

    inline void XChat::kick(const string &room, const string &user, const string &reason) {
	sendq_push(room, "/kick " + user + " " + reason);
    }
}

#endif /* _XCHAT_H_INCLUDED */
