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
    
    struct userinfo_room {
	string rid;
	string idle;
	string name;
    };
    struct userinfo_t {
	string nick;
	string name;
	string surname;
	int age;
	bool cert;
	int sex;
	int star;
	string email;
	string nick_created;
	int time_wasted;
	string last_online;
	string top_pos;
	vector<userinfo_room> rooms;
    };
}

#include "events.h"

namespace xchat {
    /*
     * Flood protection, refresh rate, max msg length and idle protection
     * 720 is optimal for flood protection
     */
    const int send_interval = 5;
    const unsigned int max_msg_length = 200;
    extern int idle_interval, recv_interval;

    /*
     * Load balancing consts
     */
    const int tries_to_rest = 5, rest_duration = 1800,
	  nextchance_interval = 30;

    /*
     * Bools
     */
    extern bool convert_smiles;

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

    class server {
	public:
	    net::sockaddr_uni sa;
	    time_t last_break;
	    int break_count;

	    server(const net::sockaddr_uni &a) : sa(a), last_break(0),
						 break_count(0)
	    { }
    };

    class XChat {
	public:
	    vector<server> servers;
	    void makeservers();

	    string uid, sid;
	    x_nick me;
	    rooms_t rooms;

	    time_t last_sent, last_recv;
	    queue<send_item> sendq;
	    deque<recv_item> recvq, old_recvq;

	    void do_sendq();
	    void fill_recvq();
	    void recvq_push(Event *e);
	    void recvq_parse_push(string m, room& r);
	    Event * recvq_pop();
	    string recode_to_client(string s);

	    XChat(const string& user, const string& pass);
	    ~XChat();

	    void join(const string& rid);
	    void leave(string rid);
	    void getmsg(room& r);
	    void putmsg(room& r, const string& target, const string& msg);
	    void setdesc(const string& rid, const string& desc);

	    int lastsrv;
	    string lastsrv_broke();
	    int makesrv();
	    string makeurl(const string& url);
	    string makeurl2(const string& url);

	    static void stripjsescapes(string &s);
	    static void striphtml(string &s);
	    static void getdate(string &m, string &date);
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
	    void kick(const string &room, const string &target, const string &reason);
	    void kill(const string &target, const string &reason);
	    void admin(const string &room, const string &newadmin);
	    void lock(const string &room);
	    void unlock(const string &room);

	    x_nick* findnick(string src, room **r);
	    bool isadmin(const string &rid, string nick);
	    bool ispermadmin(const string &rid, string nick);

	    void list(listout_t &listout);
	    bool ison(const string& nick);
	    userinfo_t userinfo(const string& nick);
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
	/*
	 * final decision on target room will be made in do_sendq
	 */
	sendq.push(send_item("", target, msg));
    }

    inline void XChat::kick(const string &room, const string &target, const string &reason) {
	sendq.push(send_item(room, "~", "/kick " + target + " " + reason));
    }

    inline void XChat::kill(const string &target, const string &reason) {
	if (rooms.size()) {
	    sendq.push(send_item(rooms.begin()->first, "~", "/kill " + target + " " + reason));
	} else {
	    throw runtime_error("Can't do KILL without channel joined");
	}
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
