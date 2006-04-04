#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#pragma interface
#include <cstdio>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <unistd.h>
#ifdef WIN32
# include <ws2tcpip.h>
# if !defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0500)
#  include <wspiapi.h>
# endif
#else
# include <netinet/in.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#define SIZEOF_SOCKADDR(so) ((so).sa.sa_family == AF_INET6 ? \
    sizeof((so).sin6) : sizeof((so).sin))

#define PORT_SOCKADDR(so) (((so).sa.sa_family == AF_INET) ? \
	((so).sin.sin_port):((so).sin6.sin6_port))

namespace net {
    using namespace std;

    union sockaddr_uni {
	struct sockaddr sa;
	struct sockaddr_in sin;
	struct sockaddr_in6 sin6;
    };

    typedef long long millitime_t;

    int input_timeout(int filedes, millitime_t ms);
    int output_timeout(int filedes, millitime_t ms);
    string tomi_ntop(const sockaddr_uni& name);
    void tomi_pton(string p, sockaddr_uni& name);
    string revers(const sockaddr_uni& name);
    void resolve(const string& hostname, const string& service, vector<sockaddr_uni> &addrs);
    millitime_t millitime();
    int thread_init();

    class timeout : public runtime_error {
	public:
	    timeout(const string& a) : runtime_error(a) { }
    };

    class TomiTCP {
	public:
	    TomiTCP();
	    TomiTCP(uint16_t port, const string& addr = "::"); // listening socket
	    TomiTCP(const string& hostname, uint16_t port); // connection
	    void listen(uint16_t port, const string& addr = "::");
	    void connect(const string& hostname, const string& service,
		    const string& bindhostname = "", const string& bindservice = "");
	    void attach(int filedes);
	    void close();
	    ~TomiTCP();
	    bool ok();
	    int nodelay();
	    TomiTCP* accept();
	    void send(const char* buf, int sz, unsigned int ms = 10);
	    int recv(char* buf, int sz, unsigned int ms = 5000);
	    FILE* makestream();
	    //int getline(string& s);
	    int getline(string& s, char delim = '\n');
	    string ident(int ms = 10000);

	    operator FILE* () { return stream; }

	    sockaddr_uni lname,rname;
	    int sock;
	    FILE *stream;

	    int conn_timeout, recv_timeout;
	    static int g_conn_timeout, g_recv_timeout;

#ifdef WIN32
	    int w32socket;
#endif

	    /*
	     * If you want your program to be portable, you MUST use netsock
	     * for functions that have something to do with sockets...
	     */
	    int &netsock;
    };
}

#endif /* NET_H_INCLUDED */
