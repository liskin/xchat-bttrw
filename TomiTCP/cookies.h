#ifndef COOKIES_H_INCLUDED
#define COOKIES_H_INCLUDED
#pragma interface
#include <map>
#include <string>

namespace net {
    using namespace std;

    struct TomiCookie {
	string name, value;
    };

    class TomiCookies {
	public:
	    TomiCookies();
	    ~TomiCookies();

	    typedef map<string,TomiCookie> cookies_t;
	    cookies_t cookies;

	    void http_setcookie(const string& cookie);
	    void http_getcookies(string& out);

	    void save(const string& file);
	    void load(const string& file);
    };
}

#endif /* COOKIES_H_INCLUDED */
