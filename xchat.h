#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <queue>
#include <deque>
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

    class send_item {
	public:
	    string room, target, msg;
	    send_item(const string& r, const string& t, const string& m) :
		room(r), target(t) {
		    if (client_charset.length())
			msg = recode(m, client_charset, "UTF-8");
		    else
			msg = m;
		}
    };

    class XChat {
	public:
	    vector<net::sockaddr_uni> servers;

	    string uid, sid, nick;
	    rooms_t rooms;

	    time_t last_sent, last_recv;
	    queue<send_item> sendq;
	    deque<recv_item> recvq, old_recvq;

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
	    void putmsg(room& r, const string& target, const string& msg);
	    void setdesc(const string& rid, const string& desc);

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
	    bool sysnoroom(string &m);
	    bool whisper_in_queue(string &m, string &src);

	    void msg(const string &room, const string &msg);
	    void whisper(const string &room, const string &target, const string &msg);
	    void whisper(const string &target, const string &msg);
	    void kick(const string &room, const string &user, const string &reason);
	    void admin(const string &room, const string &newadmin);
	    void lock(const string &room);
	    void unlock(const string &room);

	    x_nick* findnick(string src, room **r);
	    bool isadmin(const string &rid, string nick);
	    bool ispermadmin(const string &rid, string nick);

	    void list(listout_t &listout);
	    bool ison(const string& nick);
    };

    inline void XChat::recvq_push(Event *e) {
	recvq.push_back(recv_item(e));
    }

    inline Event * XChat::recvq_pop() {
	auto_ptr<Event> e = recvq.front().e; recvq.pop_front();

	/*
	 * Store EvWhisper in a secondary recvq to make whisper_in_queue able
	 * to check for existing Whispers in previous refresh. All this is
	 * needed to fix an xchat.cz race condition.
	 */
	EvWhisper *f = dynamic_cast<EvWhisper*>(e.get());
	if (f)
	    old_recvq.push_back(recv_item(new EvWhisper(*f)));

	return e.release();
    }
    
    inline void XChat::msg(const string &room, const string &msg) {
	sendq.push(send_item(room, "~", msg));
    }

    inline void XChat::whisper(const string &room, const string &target, const string &msg) {
	sendq.push(send_item(room, target, msg));
    }
    
    inline void XChat::whisper(const string &target, const string &msg) {
	if (rooms.size()) {
	    // decide if we have to send global msg
	    room *r;
	    x_nick *n = findnick(target, &r);
	    if (n)
		sendq.push(send_item(r->rid, target, msg));
	    else
		sendq.push(send_item(rooms.begin()->first,
			    "~", "/m " + target + " " + msg));
	} else {
	    throw runtime_error("Can't send PRIVMSG's without channel joined");
	}
    }

    inline void XChat::kick(const string &room, const string &user, const string &reason) {
	sendq.push(send_item(room, "~", "/kick " + user + " " + reason));
    }

    inline void XChat::admin(const string &room, const string &newadmin) {
	sendq.push(send_item(room, "~", "/admin " + newadmin));
    }
    
    inline void XChat::lock(const string &room) {
	sendq.push(send_item(room, "~", "/lock"));
    }
    
    inline void XChat::unlock(const string &room) {
	sendq.push(send_item(room, "~", "/unlock"));
    }
}

#endif /* _XCHAT_H_INCLUDED */
