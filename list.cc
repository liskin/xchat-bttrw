#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "xchat.h"
#include "TomiTCP/http.h"
#include "TomiTCP/str.h"

using namespace net;

namespace xchat {
    static bool stringcmp(const roominfo &a,
	    const roominfo &b)
    {
	if (a.registered != b.registered)
	    return a.registered;
	return strcoll(a.name.c_str(), b.name.c_str()) < 0;
    }

    /**
     * Get a list of available rooms.
     * \param listout Output array.
     * \param type Type of rooms to list, binary mask of ROOM_*.
     */
    void XChat::list(listout_t &listout, int type)
    {
	TomiHTTP s;
	string l;

	int retries = servers.size();
retry:
	try {
	    int ret = s.GET(makeurl("scripts/rooms.php"),0);
	    if (ret != 200)
		throw runtime_error("Not HTTP 200 Ok while getting list");
	} catch (runtime_error &e) {
	    if (retries--) {
		lastsrv_broke();
		goto retry;
	    } else
		throw runtime_error(string(e.what()) + " - " + lastsrv_broke());
	}

	while (s.getline(l)) {
	    wstrip(l);

	    roominfo room;

	    room.rid = string(l, 0, l.find(' '));
	    l.erase(0, room.rid.length());
	    l.erase(0, l.find_first_not_of(' '));

	    int pos = l.find(' ');
	    room.registered = (string(l, 0, pos) == "1" ? true : false);
	    l.erase(0, pos);
	    l.erase(0, l.find_first_not_of(' '));

	    pos = l.find(' ');
	    room.count = atoi(string(l, 0, pos).c_str());
	    l.erase(0, pos);
	    l.erase(0, l.find_first_not_of(' '));

	    room.name = l;

	    if ((type & ROOM_TEMPORARY && room.registered == 0) ||
		    (type & ROOM_REGISTERED && room.registered == 1))
		listout.push_back(room);
	}
	s.close();

	sort(listout.begin(), listout.end(), stringcmp);
	for (listout_t::iterator i = listout.begin(); i != listout.end(); i++)
	    i->name = recode_to_client(i->name);
    }
}
