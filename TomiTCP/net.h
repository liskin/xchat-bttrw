#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#pragma interface
#include <iostream>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <netinet/in.h>
#include <sys/types.h>
#include <exception>
#include <stdexcept>

#define SIZEOF_SOCKADDR(so) ((so).sa.sa_family == AF_INET6 ? \
    sizeof(so.sin6) : sizeof(so.sin))

namespace net {
    using namespace std;

    union sockaddr_uni {
	struct sockaddr sa;
	struct sockaddr_in sin;
	struct sockaddr_in6 sin6;
    };

    int input_timeout(int filedes, unsigned int ms);
    int output_timeout(int filedes, unsigned int ms);
    std::string tomi_ntop(const sockaddr_uni& name);
    void tomi_pton(std::string p, sockaddr_uni& name);
    std::string revers(const sockaddr_uni& name);

    class timeout : public std::runtime_error {
	public:
	    timeout(const std::string& a) : std::runtime_error(a) { }
    };

    class TomiTCP {
	public:
	    TomiTCP() : sock(-1) { }
	    TomiTCP(uint16_t port); // listening socket
	    TomiTCP(const std::string& hostname, uint16_t port); // connection
	    ~TomiTCP();
	    bool ok();
	    int nodelay();
	    TomiTCP* accept();
	    void send(const char* buf, int sz, unsigned int ms = 10);
	    int recv(char* buf, int sz, unsigned int ms = 5000);
	    FILE* makestream();

	    sockaddr_uni name;
	    int sock;
    };
}

#endif /* NET_H_INCLUDED */
