#include <iostream>
#include <string>
#include <memory>
#include <cstdio>
#include <errno.h>
#include "../net.h"
#include "../str.h"
using namespace std;
using namespace net;

const int proxy_timeout = 60;

int sendall(int s, const char* buf, const int size);
void goon(int a, int b);

TomiTCP out, srv;
auto_ptr<TomiTCP> in;

/*
 * Hop to next host
 */
char *lasthost = 0;
void pconnect(TomiTCP &c, char *host)
{
    if (!host || !*host)
	return;

    char *mylasthost = lasthost;
    lasthost = host;

    cout << "Proxy connecting to " << host << endl;
    fprintf(c, "CONNECT %s HTTP/1.0\n\n", host);

    if (input_timeout(c.sock, proxy_timeout * 1000) <= 0) {
	/*
	 * The proxy wasn't able to connect to the one we have in host var
	 * within a specified time.
	 */
	cerr << "Removing " << host << endl;
	*host = 0;
	throw runtime_error("Connection timeout");
    }

    string line;
    if (c.getline(line)) {
	chomp(line);
	char httpver[10], err[100] = ""; int code;
	int ret = sscanf(line.c_str(), "HTTP/%9[^ ] %i %99[^\n]", httpver, &code, err);
	if (ret < 2) {
	    /*
	     * The proxy replied with some unknown garbage.
	     */
	    if (mylasthost) {
		cerr << "Removing " << mylasthost << endl;
		*mylasthost = 0;
	    }
	    throw runtime_error("Parse error on HTTP response");
	}
	if (code < 200 || code > 299) {
	    /*
	     * The proxy wasn't able to connect to host we wanted.
	     */
	    if (code >= 500) {
		cerr << "Removing " << host << endl;
		*host = 0;
	    }
	    /*
	     * The proxy didn't want to connect.
	     */
	    else if (code >= 400 && mylasthost) {
		cerr << "Removing " << mylasthost << endl;
		*mylasthost = 0;
	    }

	    throw runtime_error("HTTP not ok! (" + tostr<int>(code) + " " +
		    string(err) + ")");
	}
    } else {
	/*
	 * The proxy didn't want to talk.
	 */
	if (mylasthost) {
	    cerr << "Removing " << mylasthost << endl;
	    *mylasthost = 0;
	}
	throw runtime_error("Zero sized HTTP reply");
    }

    /*
     * Skip HTTP headers.
     */
    while (c.getline(line)) {
	chomp(line);
	if (line.empty())
	    break;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
	cerr << "Usage: proxyhopper <port> [proxy:port] [proxy:port...] <host>:<port>";
	cerr << endl;
	return -1;
    }

    int arg = 1;

    try {
	srv.listen(atol(argv[arg++]));
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    while (1) {
	int marg = arg;
	try {
	    in.reset(srv.accept());

	    /*
	     * Skip removed hosts
	     */
	    while (marg < argc && !argv[marg][0])
		marg++;
	    if (marg >= argc) {
		cerr << "There are no hosts left!" << endl;
		return -1;
	    }

	    /*
	     * Parse the first host
	     */
	    lasthost = argv[marg];
	    string firsthost, firstport, h(argv[marg++]);
	    unsigned int colon = h.rfind(':');
	    if (colon == string::npos) {
		cerr << "Invalid host (" << h << ")" << endl;
		return -1;
	    }
	    firsthost.assign(h, 0, colon);
	    firstport.assign(h, colon + 1, string::npos);

	    /*
	     * Connect the first host
	     */
	    cerr << "Connecting to " << firsthost << ":" << firstport << endl;
	    try {
		out.connect(firsthost, firstport);
	    } catch (...) {
		*lasthost = 0;
		throw;
	    }

	    /*
	     * Hop over proxys
	     */
	    while (marg < argc)
		pconnect(out, argv[marg++]);

	    cerr << "Ready, Go on!" << endl;

	    /*
	     * Pass data between sockets
	     */
	    goon(in->sock, out.sock);

	} catch (runtime_error e) {
	    cerr << e.what() << endl;
	}
	out.close();
	in.reset(0);
	cerr << "Connection closed" << endl << endl;
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
