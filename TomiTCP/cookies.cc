#pragma implementation
#include "cookies.h"
#include <sstream>

namespace net {
    TomiCookies::TomiCookies()
    {
    }

    TomiCookies::~TomiCookies()
    {
    }

    void TomiCookies::http_setcookie(string cookie)
    {
	stringstream s(cookie);
	TomiCookie c;

	getline(s,c.name,'=');
	getline(s,c.value,';');

	cookies[c.name] = c;
    }

    void TomiCookies::http_getcookies(string& out)
    {
	for (cookies_t::iterator i = cookies.begin(); i != cookies.end(); i++)
	    out += "Cookie: " + i->first + "=" + i->second.value + "\r\n";
    }
}
