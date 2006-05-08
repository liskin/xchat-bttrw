#include <iostream>
#include <cstdlib>
#include "xchat.h"
#include "TomiTCP/http.h"

using namespace net;

namespace xchat {
    /**
     * Get user info from Xchat.
     * \param nick Nick of the user we want info about.
     * \return Filled in userinfo_t structure. If the nick does not exist,
     * returns empty string in the name member.
     */
    userinfo_t XChat::userinfo(const string& nick)
    {
	TomiHTTP s;

	int retries = servers.size();
retry1:
	try {
	    int ret = s.GET(makeurl("scripts/user.php?nick=" + 
			TomiHTTP::URLencode(nick)),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting userinfo");
	} catch (runtime_error &e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry1;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	userinfo_t out;
	string l;

	if (!s.getline(l))
	    return out;
	out.nick = nick;
	out.name = recode_to_client(chomp(l));
	
	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.surname = recode_to_client(chomp(l));

	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.age = atoi(l.c_str());

	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.cert = (l == "1");

	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.sex = (l == "0") ? 1 : 0;
	
	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.star = atoi(l.c_str());
	if (out.star < 0 || out.star > 5)
	    throw("Bad star ID, please contact developer team!");
	
	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.email = recode_to_client(chomp(l));

	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.nick_created = recode_to_client(chomp(l));

	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.time_wasted = atoi(l.c_str());

	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.last_online = recode_to_client(chomp(l));

	if (!s.getline(l))
	    throw runtime_error("User info is not complete.");
	out.top_pos = recode_to_client(chomp(l));

	s.close();

	// room list
	retries = servers.size();
retry2:
	try {
	    int ret = s.GET(makeurl("scripts/wonline.php?nick=" + 
			TomiHTTP::URLencode(nick)),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting where is user online");
	} catch (runtime_error &e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry2;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}
	
	if (!s.getline(l))
	    throw runtime_error("Channel list is not complete.");
	int count = atoi(l.c_str());

	for (int i = 0; i < count; i++) {
	    if (!s.getline(l))
		throw runtime_error("Channel list is not complete.");
	    
	    userinfo_room room;
	    
	    room.rid = string(l, 0, l.find(' '));
	    l.erase(0, room.rid.length());
	    l.erase(0, l.find_first_not_of(' '));

	    room.idle = string(l, 0, l.find(' '));
	    l.erase(0, room.idle.length());
	    l.erase(0, l.find_first_not_of(' '));

	    l.erase(0, l.find(' '));
	    l.erase(0, l.find_first_not_of(' '));

	    room.name = recode_to_client(chomp(l));

	    out.rooms.push_back(room);
	}

	return out;
    }
}
