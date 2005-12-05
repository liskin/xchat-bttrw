#ifndef _XCHAT_H_INCLUDED
#define _XCHAT_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <deque>
#include <memory>
#include <set>
#include "TomiTCP/net.h"
#include "TomiTCP/str.h"
#include "charset.h"

/** \brief A namespace containing classes, functions, types and variables for
 * accessing xchat.cz.
 */
namespace xchat {
    using namespace std;

    void xchat_init();

    /**
     * \brief A structure representing a nick on xchat.
     */
    struct x_nick {
	string nick; ///< Nickname in the right case.
	int sex; ///< Sex. 0 for women, 1 for men, 2 for unknown.
    };

    /**
     * \brief Type definition for nick list. It's a map of lowercase nick to
     * x_nick structure.
     */
    typedef map<string, x_nick> nicklist_t;

    /**
     * \brief A structure representing a room on xchat. Created by XChat::join
     * and saved into XChat::rooms.
     */
    struct room {
	string rid; ///< Room id as used in rid parameter to modchat.
	int l; ///< Last line as used in last_line parameter to modchat.
	nicklist_t nicklist; ///< List of nicks in the room.
	time_t last_sent; ///< Time of last sent msg to the room.
	time_t last_roominfo; ///< Time of last roominfo retrieval.
	string admin; ///< Lowercase nick of current admin of the room.
	bool locked; ///< Locked status.
	string name, ///< Room name.
	       desc; ///< Room description.
	set<string> admins; ///< List of permanent admins.
    };

    /**
     * \brief Type definition for the XChat::rooms array. It's a map of room
     * id to room structure.
     */
    typedef map<string, room> rooms_t;

    /**
     * \brief Type definition for the XChat::list output. It's an array of
     * room id => room name pairs.
     */
    typedef vector<pair<string, string> > listout_t;
    
    /**
     * \brief Structure definition for a room in userinfo_t::rooms.
     */
    struct userinfo_room {
	string rid;
	string idle;
	string name;
    };

    /**
     * \brief Structure definition for a user info. Returned by the
     * XChat::userinfo function.
     */
    struct userinfo_t {
	string nick; ///< Nickname. Not in the right case!
	string name; ///< A first name as filled in the user profile.
	string surname; ///< A surname as filled in the user profile.
	int age; ///< An age as filled in the user profile.
	bool cert; ///< User's certification status.
	int sex; ///< Sex. Values are the same as in x_nick.
	/**
	 * \brief Star (privilege level).
	 * 0 stands for blue, 1 for green, 2 for yellow, 3 for red, 4 for
	 * black.
	 */
	int star;
	string email; ///< An email address as filled in the user profile.
	string nick_created; ///< Nick creation date in unspecified format.
	int time_wasted; ///< Amount of time wasted in seconds.
	/**
	 * \brief A time in unspecified format, when the user was last seen
	 * online.
	 */
	string last_online;
	string top_pos; ///< A position in the TOP chart.
	vector<userinfo_room> rooms; ///< Array of rooms a user is in.
    };
}

#include "events.h"

namespace xchat {
    /**
     * \brief Interval between posting messages from XChat::sendq.
     * This is needed because xchat refuses to send messages more often.
     */
    const int send_interval = 5;
    /**
     * \brief Maximum single message length. XChat::do_sendq splits the
     * message if needed.
     */
    const unsigned int max_msg_length = 200;
    /**
     * \brief Interval between fetching room info. XChat::fill_recvq fetches
     * it and emits events with new info that often.
     */
    const int roominfo_interval = 15 * 60;
    /**
     * \brief The number of retries to post a message in case of xchat.cz
     * refusal. Messages that could not be posted because of other reason are
     * being reposted infinitely.
     */
    const int putmsg_retries = 5;

    /**
     * \brief The number of server failures to have a rest.
     */
    const int tries_to_rest = 3;
    /**
     * \brief Length of server rest in seconds.
     */
    const int rest_duration = 7200;
    /**
     * \brief Amount of time after which the server is given another chance
     * after a failure.
     */
    const int nextchance_interval = 30;

    /**
     * \brief A class representing a received Event. It has a mutable
     * auto_ptr<Event> member allowing to copy from a const reference.
     */
    class recv_item {
	public:
	    recv_item(const recv_item &a) : e(a.e) { }
	    /**
	     * Construct a recv_item from an Event pointer.
	     * \param f Event pointer to create recv_item from.
	     */
	    recv_item(Event *f) : e(f) { }
	    mutable auto_ptr<Event> e;
    };

    /**
     * \brief A class representing a message that has to be sent.
     */
    class send_item {
	public:
	    string room;
	    string target; ///< Target or "~".
	    string msg;
	    int retries; ///< Number of retries left.
	    /**
	     * Construct a send_item.
	     * \param r Target room id.
	     * \param t Target nick or "~".
	     * \param m Message text.
	     */
	    send_item(const string& r, const string& t, const string& m) :
		room(r), target(t), msg(m), retries(putmsg_retries) { }
    };

    /**
     * \brief A class representing one of xchat servers. Used to track
     * failures.
     */
    class server {
	public:
	    string host;
	    time_t last_break;
	    int break_count;

	    /**
	     * Construct a server from its hostname.
	     * \param a Hostname.
	     */
	    server(const string &a) : host(a), last_break(0),
				      break_count(0)
	    { }
    };

    /**
     * \brief A class representing an xchat.cz connection.
     */
    class XChat {
	public:
	    vector<server> servers; ///< Array of xchat.cz clusters.
	    int lastsrv; ///< Index of last cluster, used by #lastsrv_broke.

	    void makeservers();
	    int makesrv();
	    string makeurl(const string& url);
	    string makeurl2(const string& url);
	    string lastsrv_broke();

	    string uid; ///< User id, set by #login.
	    string sid; ///< Session id, set by #login.
	    string pass; ///< Password, saved for future #relogin.
	    x_nick me; ///< Me... Own nick and sex, if known.
	    rooms_t rooms; ///< Array of rooms we are in.

	    time_t last_sent, ///< Time of last sent message.
		   last_recv; ///< Time of last new messages retrieval.
	    deque<send_item> sendq; ///< Queue of messages to send.
	    deque<recv_item> recvq; ///< Queue of new Event objects.
	    /**
	     * \brief Queue of EvWhisper objects from previous run, filled by
	     * recvq_pop.
	     */
	    deque<recv_item> old_recvq;
	    /**
	     * \brief Random variable subtracted from #idle_interval to achieve
	     * some #idle_interval variation.
	     */
	    int idle_delta;

	    void do_sendq();
	    void fill_recvq();
	    void recvq_push(Event *e);
	    void recvq_parse_push(string m, room& r);
	    Event * recvq_pop();
	    string recode_to_client(string s);
	    string recode_from_client(string s);

	    XChat(const string& user, const string& pass);
	    ~XChat();
	    void login(const string& user, const string& pass);
	    void relogin();

	    void join(const string& rid);
	    void leave(string rid);
	    void getroominfo(room& r);
	    void getmsg(room& r);
	    bool putmsg(room& r, const string& target, const string& msg);
	    void setdesc(const string& rid, const string& desc);

	    void stripjsescapes(string &s);
	    void striphtml(string &s);
	    void getdate(string &m, string &date);
	    void getnick(string &m, string &src, string &target);
	    void unsmilize(string &s);
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

	    /*
	     * Runtime configuration:
	     */

	    int idle_interval, ///< Idler rate. Use 2400 (40 min) if in doubt.
		recv_interval; ///< New messages retrieval rate.

	    bool convert_smiles; ///< Whether to convert smiles.

	    /**
	     * \brief Local (or client) charset, useful with clients that
	     * don't support UTF-8.
	     */
	    string client_charset;
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
	sendq.push_back(send_item(room, "~", recode_from_client(msg)));
    }

    inline void XChat::whisper(const string &room, const string &target, const string &msg) {
	sendq.push_back(send_item(room, target, recode_from_client(msg)));
    }
    
    inline void XChat::whisper(const string &target, const string &msg) {
	/*
	 * final decision on target room will be made in do_sendq
	 */
	sendq.push_back(send_item("", target, recode_from_client(msg)));
    }

    inline void XChat::kick(const string &room, const string &target, const string &reason) {
	sendq.push_front(send_item(room, "~", "/kick " + target + " " +
		    recode_from_client(reason)));
    }

    inline void XChat::kill(const string &target, const string &reason) {
	if (rooms.size()) {
	    sendq.push_front(send_item(rooms.begin()->first, "~", "/kill " + target +
			" " + recode_from_client(reason)));
	} else {
	    throw runtime_error("Can't do KILL without channel joined");
	}
    }

    inline void XChat::admin(const string &room, const string &newadmin) {
	sendq.push_front(send_item(room, "~", "/admin " + newadmin));
    }
    
    inline void XChat::lock(const string &room) {
	sendq.push_front(send_item(room, "~", "/lock"));
    }
    
    inline void XChat::unlock(const string &room) {
	sendq.push_front(send_item(room, "~", "/unlock"));
    }
}

#endif /* _XCHAT_H_INCLUDED */
