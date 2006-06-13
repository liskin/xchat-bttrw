#ifndef HTTP_H_INCLUDED
#define HTTP_H_INCLUDED
#include "net.h"
#include "cookies.h"
#include <map>

namespace net {
    class TomiHTTP : public TomiTCP {
	public:
	    TomiHTTP();
	    ~TomiHTTP();

	    int GET(string address, TomiCookies* cookies);
	    int POST(string address, const string& data, TomiCookies* cookies);
	    typedef map<string,string> headers_t;
	    headers_t headers;

	    static string URLencode(const string& s);
	    static string URLdecode(const string& s);
	    
	    static int g_http_conn_timeout, g_http_recv_timeout;
    };
}

#endif /* HTTP_H_INCLUDED */
