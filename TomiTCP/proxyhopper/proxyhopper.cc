#include <iostream>
#include <string>
#include <memory>
#include <cstdio>
#include <errno.h>
#include "../net.h"
#include "../str.h"
using namespace std;
using namespace net;

int sendall(int s, const char* buf, const int size);
void goon(int a, int b);

TomiTCP out, srv;
auto_ptr<TomiTCP> in;

/*
 * Hop to next host
 */
void pconnect(TomiTCP &c, const string &host)
{
    cout << "Proxy connecting to " << host << endl;
    fprintf(c, "CONNECT %s HTTP/1.0\n\n", host.c_str());

    string line;
    if (c.getline(line)) {
	chomp(line);
	char httpver[10], err[100] = ""; int code;
	int ret = sscanf(line.c_str(), "HTTP/%9[^ ] %i %99[^\n]", httpver, &code, err);
	if (ret < 2)
	    throw runtime_error("parse error on HTTP response");
	if (code < 200 || code > 299)
	    throw runtime_error("HTTP not ok! (" + string(err) + ")");
    } else {
	throw runtime_error("zero sized HTTP reply");
    }

    while (c.getline(line)) {
	chomp(line);
	if (line.empty())
	    break;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
	cout << "Usage: proxyhopper <port> [proxy:port] [proxy:port...] <host>:<port>";
	cout << endl;
	return -1;
    }

    int arg = 1;

    try {
	srv.listen(atol(argv[arg++]));

	string firsthost, firstport;
	{
	    string h = argv[arg++];
	    unsigned int colon = h.rfind(':');
	    if (colon == string::npos)
		throw runtime_error("Invalid host");
	    firsthost.assign(h, 0, colon);
	    firstport.assign(h, colon + 1, string::npos);
	}

	while (1) {
	    in.reset(srv.accept());

	    /*
	     * Connect the first host
	     */
	    cout << "Connecting to " << firsthost << ":" << firstport << endl;
	    out.connect(firsthost, firstport);

	    /*
	     * Hop over proxys
	     */
	    for (int marg = arg; marg < argc; pconnect(out, argv[marg++]));

	    /*
	     * Pass data between sockets
	     */
	    goon(in->sock, out.sock);

	    cout << "Connection closed" << endl << endl;
	    out.close();
	    in->close();
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}

int sendall(int s, const char* buf, const int size)
{
    int sz = size;
    const char *p = buf;
    while (sz) {
	int ret = TEMP_FAILURE_RETRY(write(s, p, sz));
	if (ret == -1)
	    return ret;

	p += ret;
	sz -= ret;
    }

    return size;
}

void goon(int a, int b)
{
    char buffer[4096];

    fd_set set;
    while (1) {
	FD_ZERO(&set);
	FD_SET(a, &set);
	FD_SET(b, &set);

	int ret = TEMP_FAILURE_RETRY(select((a >? b) + 1, &set, 0, 0, 0));
	if (ret == -1)
	    throw runtime_error("error in select: " + string(strerror(errno)));

	if (FD_ISSET(a, &set)) {
	    int sz = TEMP_FAILURE_RETRY(read(a, buffer, 4096));
	    if (sz == -1)
		throw runtime_error("error in read: " + string(strerror(errno)));
	    else if (!sz)
		break;

	    sz = sendall(b, buffer, sz);
	    if (sz == -1)
		throw runtime_error("error in write: " + string(strerror(errno)));
	}

	if (FD_ISSET(b, &set)) {
	    int sz = TEMP_FAILURE_RETRY(read(b, buffer, 4096));
	    if (sz == -1)
		throw runtime_error("error in read: " + string(strerror(errno)));
	    else if (!sz)
		break;

	    sz = sendall(a, buffer, sz);
	    if (sz == -1)
		throw runtime_error("error in write: " + string(strerror(errno)));
	}
    }
}
