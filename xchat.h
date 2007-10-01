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
#include "TomiTCP/http.h"
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
	string nick; ///< Nickname in the right case (hopefully).
	int sex; ///< Sex. 0 for women, 1 for men, 2 for unknown.

	bool operator == (const struct x_nick &n) {
	    return strtolower_nr(nick) == strtolower_nr(n.nick);
	}
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
	       desc, ///< Room description.
	       web;  ///< Room web.
	set<string> admins; ///< List of permanent admins.
    };

    /**
     * \brief Type definition for the XChat::rooms array. It's a map of room
     * id to room structure.
     */
    typedef map<string, room> rooms_t;

    /**
     * \brief Structure definition for a room in list output.
     */
    struct roominfo {
	string rid; ///< Room id.
	string name; ///< Room name.
	int count; ///< User count.
	bool registered; ///< Is room registered?
    };

    /**
     * \brief Type definition for the XChat::list output. It's an array of
     * roominfo structures.
     */
    typedef vector<roominfo> listout_t;

    /**
     * \brief Structure definition for a room in userinfo_t::rooms.
     */
    struct userinfo_room {
	string rid; ///< Room id.
	string idle; ///< Idle time in unspecified format.
	string name; ///< Room name.
    };

    /**
     * \brief Structure definition for a user info. Returned by the
     * XChat::userinfo function.
     */
    struct userinfo_t {
	string nick; ///< Nickname. Not necessarily in the right case.
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
	string icq; ///< An ICQ contact.
	vector<userinfo_room> rooms; ///< Array of rooms a user is in.
    };

    /**
     * \brief Structure definition for a superadmin info.
     */
    struct superadmin_t {
	string nick; ///< Nickname in the right case (hopefully).
	int sex; ///< Sex. Values are the same as in x_nick.
	/**
	 * \brief Star (privilege level).
	 * 0 stands for blue, 1 for green, 2 for yellow, 3 for red, 4 for
	 * black.
	 */
	int star;
	bool online; ///< Is admin online?
    };

    /**
     * \brief Type definition for the XChat::superadmins array. It's a map of
     * nicks to superadmin_t structure.
     */
    typedef map<string, superadmin_t> superadmins_t;
}

#include "events.h"

namespace xchat {
    /**
     * \brief Interval between posting messages from XChat::sendq.
     * This is needed because xchat refuses to send messages more often.
     */
    const int send_interval = 3;
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
     * \brief Domain name of fallback server.
     */
    const string fallback_server = "xchat.centrum.cz";
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
     * \brief Interval between reloading superadmins list.
     */
    const time_t superadmins_reload_interval = 3600;

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
	    recv_item(auto_ptr<Event> f) : e(f) { }
	    mutable auto_ptr<Event> e;
    };

    /**
     * \brief A class representing a message that has to be sent.
     */
    class send_item {
	public:
	    string room; ///< Target room.
	    string target; ///< Target nick or "~".
	    string msg; ///< The message.
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

    enum server_type {
        SERVER_MODCHAT = 0,
        SERVER_SCRIPTS = 1
    };

    enum path_type {
        PATH_PLAIN = 0,
        PATH_AUTH = 1,
        PATH_STATIC = 2
    };

    /**
     * \brief A class representing one of xchat servers. Used to track
     * failures.
     */
    class server {
        public:
            /**
             * \brief Map of types to hostnames.
             */
            typedef map<server_type, string> types_t;

            net::sockaddr_uni host; ///< Host.
            types_t types; ///< Types and hostnames.
            time_t last_break; ///< Timestamp of last failure.
            int total_break_count; ///< Number of failures so far (summary).
            int break_count; ///< Number of failures so far.

            server() : last_break(0), total_break_count(0), break_count(0)
            { }

            bool supports(server_type type) {
                return types.find(type) != types.end();
            }
    };

    /**
     * \brief A class representing an xchat.cz connection.
     */
    class XChat {
	public:
	    vector<server> servers; ///< Array of xchat.cz clusters.
	    int lastsrv; ///< Index of last cluster, used by #lastsrv_broke.
	    
            void makeservers();
	    int makesrv(server_type type);
	    string lastsrv_broke();
            string makepath(const string& path, path_type type);
            int request_GET(net::TomiHTTP &s, server_type st, const string& path,
                    path_type pt);
            int request_POST(net::TomiHTTP &s, server_type st, const string& path,
                    path_type pt, const string &data);

	    string uid; ///< User id, set by #login. Used for queries.
	    string sid; ///< Session id, set by #login. Used for queries.
	    string pass; ///< Password, saved for future #relogin.
	    x_nick me; ///< Me... Own nick and sex, if known.
	    rooms_t rooms; ///< Array of rooms we are in.

	    superadmins_t superadmins; ///< Array of superadmins (xchat admins).

	    time_t last_sent, ///< Time of last sent message.
		   last_recv; ///< Time of last new messages retrieval.
	    deque<send_item> sendq; ///< Queue of messages to send.
	    deque<recv_item> recvq; ///< Queue of new Event objects.
	    /**
	     * \brief Queue of some objects from previous run, filled by
	     * #recvq_pop. Needed to avoid duplicate messages.
	     */
	    deque<recv_item> old_recvq;
	    /**
	     * \brief Random variable subtracted from #idle_interval to achieve
	     * some variation.
	     */
	    int idle_delta;

	    void do_sendq();
	    void fill_recvq();
	    void recvq_push(auto_ptr<Event> e);
	    void recvq_parse_push(string m, room& r);
	    void recvq_parse_push_history(string m, room& r);
	    auto_ptr<Event> recvq_pop();
	    string recode_to_client(string s);
	    string recode_from_client(string s);

	    XChat(const string& user, const string& pass);
	    ~XChat();
	    void login(const string& user, const string& pass);
	    void relogin();

	    void join(const string& rid);
	    void leave(string rid);
	    void getroominfo(room& r);
	    void getmsg(room& r, bool first = false);
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
	    bool iskill(room& r, string &m, string &src, string &who, int &sex);
	    bool isadvert(string &m, string &link);
	    bool sysnoroom(string &m, bool &stopdup);
	    bool duplicate_in_queue(Event *e);

	    void msg(const string &room, const string &msg);
	    void whisper(const string &room, const string &target, const string &msg);
	    void whisper(const string &target, const string &msg);
	    void kick(const string &room, const string &target, const string &reason);
	    void kill(const string &target, const string &reason);
	    void admin(const string &room, const string &newadmin);
	    void lock(const string &room);
	    void unlock(const string &room);

	    x_nick* findnick(string src, room **r);
	    bool isin(const string &rid, string nick);
	    bool isadmin(const string &rid, string nick);
	    bool ispermadmin(const string &rid, string nick);
	    bool issuperadmin(const string nick);

	    void list(listout_t &listout, int type);
	    bool ison(const string& nick);
	    void loadsuperadmins();
	    void reloadsuperadmins();
	    userinfo_t userinfo(const string& nick);

	    /*
	     * Runtime configuration:
	     */

	    int idle_interval, ///< Idler rate. Use 2400 (40 min) if in doubt.
		recv_interval; ///< New messages retrieval rate.

	    /**
	     * \brief Whether to convert smiles and how. 1 is for normal
	     * conversion, 2 prints the smile number too.
	     */
	    int convert_smiles;

	    /**
	     * \brief Whether to really leave rooms and logout from xchat.
	     */
	    bool really_logout;

	    /**
	     * \brief Whether to post watch notices as global notices.
	     */
	    bool watch_global;

	    /**
	     * \brief Whether to show history after room join.
	     */
	    bool show_history;

	    /**
	     * \brief Local (or client) charset, useful with clients that
	     * don't support UTF-8.
	     */
	    string client_charset;

	    /**
	     * \brief Has the information about new note been already emitted?
	     */
	    bool note_emitted;

	    /**
	     * \brief Time of last superadmins reload.
	     */
	    time_t last_superadmins_reload;
    };

    /**
     * Room types
     */
    const int ROOM_TEMPORARY = 1;  // Temporary room
    const int ROOM_REGISTERED = 2; // Registered room

    /**
     * Push Event to the #recvq. The auto_ptr takes control of the pointer.
     * \param e Event to be pushed.
     */
    inline void XChat::recvq_push(auto_ptr<Event> e) {
	if (e->stopdup && duplicate_in_queue(e.get()))
	    return;

	recvq.push_back(recv_item(e));
    }

    /**
     * Pop Event from #recvq. The auto_ptr releases the pointer and it's up to
     * the caller to free it.
     * \return The released pointer.
     */
    inline auto_ptr<Event> XChat::recvq_pop() {
	auto_ptr<Event> e = recvq.front().e; recvq.pop_front();

	/*
	 * Put the events with stopdup flag to the secondary recvq to make
	 * duplicate_in_queue able to check for them in next refresh. This is
	 * needed because the event may come in the middle of room refreshing.
	 */
	if (e->stopdup)
	    old_recvq.push_back(recv_item(auto_ptr<Event>((Event*)e->clone())));

	return e;
    }
    
    /**
     * Shorthand for pushing a room message to the #sendq.
     * \param room Target room.
     * \param msg Message.
     */
    inline void XChat::msg(const string &room, const string &msg) {
	sendq.push_back(send_item(room, "~", recode_from_client(msg)));
    }

    /**
     * Shorthand for pushing a room whisper message to the #sendq.
     * \param room Target room.
     * \param target Target nick.
     * \param msg Message.
     */
    inline void XChat::whisper(const string &room, const string &target, const string &msg) {
	sendq.push_back(send_item(room, target, recode_from_client(msg)));
    }
    
    /**
     * Shorthand for pushing a whisper message to the #sendq. The target room
     * will be determined in #do_sendq.
     * \param target Target nick.
     * \param msg Message.
     */
    inline void XChat::whisper(const string &target, const string &msg) {
	/*
	 * final decision on target room will be made in do_sendq
	 */
	sendq.push_back(send_item("", target, recode_from_client(msg)));
    }

    /**
     * Shorthand for pushing a room kick to the #sendq.
     * \param room Target room.
     * \param target Target nick.
     * \param reason Kick reason.
     */
    inline void XChat::kick(const string &room, const string &target, const string &reason) {
	sendq.push_front(send_item(room, "~", "/kick " + target + " " +
		    recode_from_client(reason)));
    }

    /**
     * Shorthand for pushing a kill to the #sendq. It will be delivered using
     * the first room you have joined and will not work without having joined
     * at least one room.
     * \param target Target nick.
     * \param reason Kill reason.
     */
    inline void XChat::kill(const string &target, const string &reason) {
	if (rooms.size()) {
	    sendq.push_front(send_item(rooms.begin()->first, "~", "/kill " + target +
			" " + recode_from_client(reason)));
	} else {
	    throw runtime_error("Can't do KILL without channel joined");
	}
    }

    /**
     * Shorthand for pushing an admin change message to the #sendq.
     * \param room Target room.
     * \param newadmin Nick of the new admin.
     */
    inline void XChat::admin(const string &room, const string &newadmin) {
	sendq.push_front(send_item(room, "~", "/admin " + newadmin));
    }

    /**
     * Shorthand for pushing a room lock message to the #sendq.
     * \param room Target room.
     */
    inline void XChat::lock(const string &room) {
	sendq.push_front(send_item(room, "~", "/lock"));
    }

    /**
     * Shorthand for pushing a room unlock message to the #sendq.
     * \param room Target room.
     */
    inline void XChat::unlock(const string &room) {
	sendq.push_front(send_item(room, "~", "/unlock"));
    }
}

#endif /* _XCHAT_H_INCLUDED */
