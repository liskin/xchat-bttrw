#include "http.h"
#include "str.h"
#include "version.h"
#include <cstdio>
#include <sstream>
#include <cctype>

namespace net {
    int TomiHTTP::g_http_conn_timeout = 120,
	TomiHTTP::g_http_recv_timeout = 30;

    char *TomiHTTP::proxyhost = getenv("PROXYHOST"),
         *TomiHTTP::proxyport = getenv("PROXYPORT");

    TomiHTTP::TomiHTTP()
    {
	conn_timeout = g_http_conn_timeout;
	recv_timeout = g_http_recv_timeout;
    }

    TomiHTTP::~TomiHTTP()
    {
    }

    void TomiHTTP::parseaddr(string address, string &host, int &port, string &path)
    {
	if (string(address, 0, 7).compare(string("http://"))) {
	    throw runtime_error("Not a http URL!");
	}
	address.erase(0, 7);

	string::size_type delim = address.find('/');
	if (delim == string::npos) {
	    address += '/';

	    delim = address.length() - 1;
	    if (delim < 1) {
		throw runtime_error("Not an http URL!");
	    }
	}

	path.assign(address, delim, address.length() - delim);
	host.assign(address, 0, delim);
	address = host;

	delim = address.find(':');
	if (delim != string::npos) {
	    host.assign(address, 0, delim);
	    port = atoi(address.c_str() + delim + 1);
	}
	if (port < 1)
	    port = 80;
    }
            
    void TomiHTTP::doconnect(const string &host, int port) {
	if (proxyhost && proxyport)
	    connect(proxyhost, proxyport);
	else
	    connect(host, tostr<unsigned int>(port));
    }
    
    void TomiHTTP::doGET(const string &host, int port, string path, TomiCookies* cookies)
    {
	headers.clear();

        if (proxyhost && proxyport)
            path = "http://" + host + ":" + tostr<int>(port) + path;

	fprintf(stream,
		"GET %s HTTP/1.0\r\n"
		"User-Agent: TomiTCP/" PACKAGE_VERSION "\r\n"
		"Host: %s:%i\r\n"
		"Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n"
		"Accept-Charset: utf-8;q=0.7,*;q=0.7\r\n"
		"Connection: close\r\n",
		path.c_str(), host.c_str(), port);
	/* cookies... */
	if (cookies) {
	    string c;
	    cookies->http_getcookies(c);
	    fprintf(stream, "%s", c.c_str());
	}
	fprintf(stream, "\r\n");
        fseek(stream, 0, SEEK_CUR);
    }
            
    void TomiHTTP::doPOST(const string &host, int port, string path,
            const string& data, TomiCookies* cookies)
    {
	headers.clear();

        if (proxyhost && proxyport)
            path = "http://" + host + ":" + tostr<int>(port) + path;

	fprintf(stream,
		"POST %s HTTP/1.0\r\n"
		"User-Agent: TomiTCP/" PACKAGE_VERSION "\r\n"
		"Host: %s:%i\r\n"
		"Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n"
		"Accept-Charset: utf-8;q=0.7,*;q=0.7\r\n"
		"Connection: close\r\n"
		"Content-type: application/x-www-form-urlencoded\r\n"
		"Content-length: %i\r\n",
		path.c_str(), host.c_str(), port, (int)data.length());
	/* cookies... */
	if (cookies) {
	    string c;
	    cookies->http_getcookies(c);
	    fprintf(stream, "%s", c.c_str());
	}
	fprintf(stream, "\r\n");
	fprintf(stream, "%s", data.c_str());
        fseek(stream, 0, SEEK_CUR);
    }

    int TomiHTTP::parseresponse(TomiCookies* cookies) {
	int ret;

	string line;
	if (getline(line)) {
	    chomp(line);

	    string::size_type space = line.find(' ');
	    if (space == string::npos) {
		// Simple response
		/* This line will be discarded, next getline will not return
		 * it, because the current API can't do it. Let's hope no-one
		 * cares about the first line anyway.
		 */
		return 200;
	    }

	    line.erase(0, space + 1);

	    space = line.find(' ');
	    if (space == string::npos) {
		space = line.length();
	    }

	    string rets(line, 0, space);
	    ret = atoi(rets.c_str());
	    if (!ret) {
		// Simple response
		/* See ^^^ */
		return 200;
	    }
	} else {
	    throw runtime_error("zero sized HTTP reply");
	}

	while (getline(line)) {
	    chomp(line);
	    if (line.empty())
		break;

            string::size_type delim = line.find(':');
	    if (delim != string::npos) {
		string a(line, 0, delim), b(line, delim + 1);

		b.erase(0, b.find_first_not_of(" \t"));

		strtolower(a);
		headers[a] = b;

		if (cookies && a == "set-cookie") {
		    cookies->http_setcookie(b);
		}
	    }
	}
	
	return ret;
    }

    int TomiHTTP::GET(string address, TomiCookies* cookies)
    {
	string host, path;
	int port = 0;

        parseaddr(address, host, port, path);
        doconnect(host, port);
        doGET(host, port, path, cookies);
        return parseresponse(cookies);
    }

    int TomiHTTP::POST(string address, const string& data, TomiCookies* cookies)
    {
	string host, path;
	int port = 0;

        parseaddr(address, host, port, path);
        doconnect(host, port);
        doPOST(host, port, path, data, cookies);
        return parseresponse(cookies);
    }

    inline bool URLneedencode(unsigned char c)
    {
	if (c <= 0x1f || c >= 0x7f)
	    return 1;

	switch (c) {
	    case 0x24:
	    case 0x26:
	    case 0x2b:
	    case 0x2c:
	    case 0x2f:
	    case 0x3a:
	    case 0x3b:
	    case 0x3d:
	    case 0x3f:
	    case 0x40:
	    case 0x20:
	    case 0x22:
	    case 0x3c:
	    case 0x3e:
	    case 0x23:
	    case 0x25:
	    case 0x7b:
	    case 0x7d:
	    case 0x7c:
	    case 0x5c:
	    case 0x5e:
	    case 0x7e:
	    case 0x5b:
	    case 0x5d:
	    case 0x60:
		return 1;
	}

	return 0;
    }

    string TomiHTTP::URLencode(const string& s)
    {
	string out;

	for (string::const_iterator i = s.begin(); i != s.end(); i++) {
	    if (URLneedencode((unsigned char) *i)) {
		char c[5];
		sprintf(c,"%%%.2hhX",(unsigned char)*i);
		out += c;
	    } else {
		out += *i;
	    }
	}

	return out;
    }
    
    string TomiHTTP::URLdecode(const string& s)
    {
	string out;

	for (string::const_iterator i = s.begin(); i != s.end(); i++) {
	    char a = 0, b = 0;
	    if (*i == '%' && (i + 2) < s.end() && isxdigit(a = *(i + 1)) &&
		    isxdigit(b = *(i + 2))) {
		if (a <= '9')
		    a -= '0';
		else
		    a = (a & 7) + 9;

		if (b <= '9')
		    b -= '0';
		else
		    b = (b & 7) + 9;

		out += (char) a * 16 + b;
		i += 2;
	    } else {
		out += *i;
	    }
	}

	return out;
    }
}
