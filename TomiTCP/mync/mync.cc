#include <iostream>
#include "../net.h"
using namespace std;

int sendall(int s, const char* buf, const int size)
{
    int sz = size;
    const char *p = buf;
    while (sz) {
	int ret = TEMP_FAILURE_RETRY(write(s,p,sz));
	if (ret == -1) {
	    return -1;
	}
	p += ret;
	sz -= ret;
    }

    return size;
}

int main(int argc, char *argv[])
{
    string host,port;
    int lport = 0;
    bool listen = false;
    bool verbose = false;

    {
	int arg = 1;
	while (arg < argc) {
	    if (!strcmp(argv[arg],"-l"))
		listen = 1;
	    else if (!strcmp(argv[arg],"-v"))
		verbose = 1;
	    else if (!strcmp(argv[arg],"-p")) {
		if (++arg == argc) {
		    cerr << "Parameter needs option" << endl;
		    return -1;
		}
		lport = atol(argv[arg]);
	    } else if (argv[arg][0] != '-' && !host.length())
		host = argv[arg];
	    else if (argv[arg][0] != '-' && !port.length())
		port = argv[arg];
	    else {
		cerr << "Unrecognized option" << endl;
		return -1;
	    }
	    arg++;
	}
    }

    net::TomiTCP *c = 0;
    try {
	if (listen) {
	    if (!lport) {
		cerr << "Need local port" << endl;
		return -1;
	    }
	    net::TomiTCP s(lport);
	    c = s.accept();
	    if (verbose)
		cerr << "Connection from " << tomi_ntop(c->rname) << ":" <<
		    (int)ntohs(PORT_SOCKADDR(c->rname)) << endl;
	} else {
	    if (!host.length() || !port.length()) {
		cerr << "Need host and port" << endl;
		return -1;
	    }
	    c = new net::TomiTCP;
	    c->connect(host,port);
	    if (verbose)
		cerr << "Connected to " << host << ":" << port << endl;
	}

	char buffer[4096];

	while (1) {
	    fd_set set;
	    timeval timeout;
	    timeout.tv_sec = 1;
	    timeout.tv_usec = 0;

	    FD_ZERO(&set);
	    FD_SET(0, &set);
	    FD_SET(c->sock, &set);

	    int ret = TEMP_FAILURE_RETRY(select(FD_SETSIZE,&set,NULL,NULL,NULL));
	    if (ret == -1)
		throw runtime_error("error in select: "+string(strerror(errno)));

	    if (FD_ISSET(0, &set)) {
		int sz = TEMP_FAILURE_RETRY(read(0,buffer,4096));
		if (sz == -1)
		    throw runtime_error("error in read: "+string(strerror(errno)));
		if (!sz)
		    break;

		sz = sendall(c->sock,buffer,sz);
		if (sz == -1)
		    throw runtime_error("error in write: "+string(strerror(errno)));
	    }

	    if (FD_ISSET(c->sock, &set)) {
		int sz = TEMP_FAILURE_RETRY(read(c->sock,buffer,4096));
		if (sz == -1)
		    throw runtime_error("error in read: "+string(strerror(errno)));
		if (!sz)
		    break;

		sz = sendall(1,buffer,sz);
		if (sz == -1)
		    throw runtime_error("error in write: "+string(strerror(errno)));
	    }
	}

	delete c;
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	if (c)
	    delete c;
	return -1;
    }

    return 0;
}
