#ifndef _EVENTS_H_INCLUDED
#define _EVENTS_H_INCLUDED

#include <string>

namespace xchat {
    using namespace std;

    class XChat;
    struct room;
    struct x_nick;

    /*
     * Definition of Event tree. Events don't have any identifier, they are
     * meant to be distinguished by RTTI.
     */

    class Event {
	    friend class XChat;
	protected:
	    string s;
	public:
	    virtual const string & str() { return s; }
	    virtual ~Event() {}
    };

    class EvRoomOther : public Event {
	    friend class XChat;
	protected:
	    string rid;
	public:
	    virtual const string & getrid() { return rid; }
	    virtual ~EvRoomOther() {}
    };
    
    class EvRoomError : public EvRoomOther {
	    friend class XChat;
	private:
	    string s2;
	public:
	    virtual const string & str() { s2 = rid + ": " + s; return s2; }
	    virtual ~EvRoomError() {}
    };
    
    class EvRoomMsg : public EvRoomOther {
	    friend class XChat;
	protected:
	    x_nick src;
	public:
	    virtual const x_nick & getsrc() { return src; }
	    virtual ~EvRoomMsg() {}
    };
    
    class EvRoomWhisper : public EvRoomOther {
	    friend class XChat;
	protected:
	    x_nick src;
	    string target;
	public:
	    virtual const x_nick & getsrc() { return src; }
	    virtual const string & gettarget() { return target; }
	    virtual ~EvRoomWhisper() {}
    };
    
    class EvRoomJoin : public EvRoomOther {
	    friend class XChat;
	protected:
	    x_nick src;
	public:
	    virtual const x_nick & getsrc() { return src; }
	    virtual ~EvRoomJoin() {}
    };
    
    class EvRoomLeave : public EvRoomOther {
	    friend class XChat;
	protected:
	    x_nick src;
	    string reason;
	public:
	    virtual const x_nick & getsrc() { return src; }
	    virtual const string & getreason() { return reason; }
	    virtual ~EvRoomLeave() {}
    };
    
    class EvRoomKick : public EvRoomOther {
	    friend class XChat;
	protected:
	    x_nick src;
	    x_nick target;
	    string reason;
	public:
	    virtual const x_nick & getsrc() { return src; }
	    virtual const string & getreason() { return reason; }
	    virtual const x_nick & gettarget() { return target; }
	    virtual ~EvRoomKick() {}
    };
    
    class EvRoomAdvert : public EvRoomOther {
	    friend class XChat;
	protected:
	    string link;
	public:
	    virtual const string & getlink() { return link; }
	    virtual ~EvRoomAdvert() {}
    };
    
    class EvRoomSysMsg : public EvRoomOther {
	    friend class XChat;
	public:
	    virtual ~EvRoomSysMsg() {}
    };
    
    class EvRoomSysText : public EvRoomOther {
	    friend class XChat;
	public:
	    virtual ~EvRoomSysText() {}
    };
    
    class EvRoomIdlerMsg : public EvRoomOther {
	    friend class XChat;
	public:
	    virtual ~EvRoomIdlerMsg() {}
    };
    
    class EvRoomAdminChange : public EvRoomOther {
	    friend class XChat;
	protected:
	    string before, now, s2;
	public:
	    virtual const string & str() {
		s2 = "Admin change: " + before + " => " + now;
		return s2;
	    }
	    virtual const string & getbefore() { return before; }
	    virtual const string & getnow() { return now; }
	    virtual ~EvRoomAdminChange() {}
    };

    class EvRoomLockChange : public EvRoomOther {
	    friend class XChat;
	protected:
	    bool before, now;
	    string s2;
	public:
	    virtual const string & str() {
		s2 = string("Locked change: ") + (before?"locked":"unlocked") +
		    " => " + (now?"locked":"unlocked");
		return s2;
	    }
	    virtual const bool & getbefore() { return before; }
	    virtual const bool & getnow() { return now; }
	    virtual ~EvRoomLockChange() {}
    };
    
    class EvError : public Event {
	    friend class XChat;
	public:
	    virtual ~EvError() {}
    };
    
    class EvSysMsg : public Event {
	    friend class XChat;
	public:
	    virtual ~EvSysMsg() {}
    };

    class EvWhisper : public Event {
	    friend class XChat;
	protected:
	    x_nick src;
	    string target;
	public:
	    virtual const x_nick & getsrc() { return src; }
	    virtual const string & gettarget() { return target; }
	    virtual ~EvWhisper() {}
    };
}

#include "xchat.h"

#endif
