#ifndef HTTP_H_INCLUDED
#define HTTP_H_INCLUDED
#pragma interface
#include "net.h"
#include "cookies.h"
#include <map>

namespace net {
    class TomiHTTP : public TomiTCP {
	public:
	    TomiHTTP();
	    ~TomiHTTP();

	    int GET(string address, TomiCookies* cookies);
	    int POST(string address, string data, TomiCookies* cookies);
	    map<string,string> headers;
    };
}

#endif /* HTTP_H_INCLUDED */
