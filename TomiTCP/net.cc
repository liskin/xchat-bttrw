#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include "net.h"
#ifndef WIN32
# include <netinet/tcp.h>
# include <sys/time.h>
#else
# include <io.h>
# include <mswsock.h>
# include <sys/types.h>
# include <sys/timeb.h>
#endif
#include "str.h"

#ifndef TEMP_FAILURE_RETRY
# define TEMP_FAILURE_RETRY(a) (a)
#endif

#ifndef SOL_IPV6
# define SOL_IPV6 IPPROTO_IPV6
#endif

#ifdef WIN32
# define EAFNOSUPPORT WSAEAFNOSUPPORT
# define EINPROGRESS WSAEWOULDBLOCK
#endif

namespace net {
    /*
     * We don't want to break while (getline()), so g_recv_timeout is zero.
     */
    int TomiTCP::g_conn_timeout = 120, TomiTCP::g_recv_timeout = 0;
    
    int thread_init_tmp = thread_init();
    int thread_init()
    {
#ifdef WIN32
	winsock_init();
#endif
	return 0;
    }

    TomiTCP::TomiTCP() : sock(-1), stream(0),
			 conn_timeout(g_conn_timeout), recv_timeout(g_recv_timeout)
#ifdef WIN32
			 , w32socket(-1), netsock(w32socket)
#else
			 , netsock(sock)
#endif
    {
	memset(&lname,0,sizeof(lname));
	memset(&rname,0,sizeof(rname));
    }

    TomiTCP::TomiTCP(uint16_t port, const string &addr) : sock(-1), stream(0),
		     conn_timeout(g_conn_timeout), recv_timeout(g_recv_timeout)
#ifdef WIN32
		     , w32socket(-1), netsock(w32socket)
#else
		     , netsock(sock)
#endif
    {
	memset(&lname,0,sizeof(lname));
	memset(&rname,0,sizeof(rname));
	listen(port,addr);
    }

    void TomiTCP::listen(uint16_t port, const string &addr)
    {
	close();

	memset(&lname,0,sizeof(lname));
	lname.sa.sa_family = AF_INET6;
	try { tomi_pton(addr,lname); } catch (runtime_error e) {
	    if (addr == "::")
		tomi_pton("0.0.0.0",lname);
	    else
		throw;
	}
	PORT_SOCKADDR(lname) = htons(port);

	netsock = ::socket(lname.sa.sa_family, SOCK_STREAM, 0);
	if (netsock < 0 && (sock_errno == EINVAL || sock_errno == EAFNOSUPPORT) && addr == "::") {
                lname.sin.sin_family = AF_INET;
                lname.sin.sin_addr.s_addr = INADDR_ANY;
                
                netsock = ::socket(PF_INET, SOCK_STREAM, 0);
        }
	if (netsock < 0) {
	    throw runtime_error("socket: " + string(strerror(sock_errno)));
	}

	int set_opt = 1;
#ifndef WIN32
        if (::setsockopt(netsock,SOL_SOCKET,SO_REUSEADDR,(char*)&set_opt,sizeof(set_opt)))
	    cerr << "Could not set SO_REUSEADDR" << endl;
#endif

	if (lname.sa.sa_family == AF_INET6) {
	    set_opt = 0;
	    if (::setsockopt(netsock,SOL_IPV6,IPV6_V6ONLY,(char*)&set_opt,sizeof(set_opt)))
		cerr << "Could not set IPV6_V6ONLY" << endl;
	}

	if (::bind(netsock,&lname.sa,SIZEOF_SOCKADDR(lname))) {
	    int er = sock_errno;
	    close();
	    throw runtime_error("bind: " + string(strerror(er)));
	}

	if (::listen(netsock,5)) {
	    int er = sock_errno;
	    close();
	    throw runtime_error("listen: " + string(strerror(er)));
	}
    }

    TomiTCP::TomiTCP(const string& hostname, uint16_t port) : sock(-1), stream(0),
		     conn_timeout(g_conn_timeout), recv_timeout(g_recv_timeout)
#ifdef WIN32
		     , w32socket(-1), netsock(w32socket)
#else
		     , netsock(sock)
#endif
    {
	memset(&lname, 0, sizeof(lname));
	memset(&rname, 0, sizeof(rname));
	connect(hostname, tostr<unsigned int>(port));
    }

    void TomiTCP::connect(const string& hostname, const string& service,
	    const string& bindhostname, const string& bindservice)
    {
	close();

	vector<sockaddr_uni> addrs, bindaddrs;
	resolve(hostname, service, addrs);
	if (bindhostname.length() || bindservice.length())
	    resolve(bindhostname, bindservice, bindaddrs);

	string err = "No address to connect to";

	for (vector<sockaddr_uni>::iterator i = addrs.begin(); i != addrs.end(); i++) {
	    memcpy(&rname,&(*i),SIZEOF_SOCKADDR(*i));

	    netsock = ::socket(rname.sa.sa_family, SOCK_STREAM, 0);
	    if (netsock < 0) {
		err = "socket: " + string(strerror(sock_errno));
	    } else {
		for (vector<sockaddr_uni>::iterator j = bindaddrs.begin();
			j != bindaddrs.end(); j++)
		    if (j->sa.sa_family == rname.sa.sa_family)
			if (!::bind(netsock, &j->sa, SIZEOF_SOCKADDR(*j)))
			    break;

		try { set_nonblock_flag(1); }
		catch (runtime_error e) {
		    close();
		    throw;
		}

		int ret;
		ret = TEMP_FAILURE_RETRY(::connect(netsock,
			    (const sockaddr*)&rname, SIZEOF_SOCKADDR(rname)));
		if (ret && sock_errno == EINPROGRESS) {
		    ret = connect_timeout(netsock, conn_timeout * 1000);
		    if (ret > 0) {
			int er;
			socklen_t len = sizeof(er);
			if (getsockopt(netsock, SOL_SOCKET, SO_ERROR, (char*) &er, &len)) {
			    int er = sock_errno;
			    close();
			    throw runtime_error("getsockopt: " + string(strerror(er)));
			}
			if (er) {
			    close();
			    err = "connect: " + string(strerror(er));
			} else {
			    try { set_nonblock_flag(0); }
			    catch (runtime_error e) {
				close();
				throw;
			    }
			    break;
			}
		    } else if (ret < 0) {
			int er = sock_errno;
			close();
			throw runtime_error("connect_timeout: " + string(strerror(er)));
		    } else {
			close();
			err = "connect timeout";
		    }
		} else if (ret) {
		    int er = sock_errno;
		    close();
		    err = "connect: " + string(strerror(er));
		} else {
		    close();
		    err = "connect: success?";
		}
	    }
	}

	if (netsock < 0) {
	    if (err == "connect timeout")
		throw timeout(err);
	    else
		throw runtime_error(err);
	}

	socklen_t len = SIZEOF_SOCKADDR(lname);
	if (getsockname(netsock,(sockaddr*)&lname,&len)) {
	    throw runtime_error("getsockname: " + string(strerror(sock_errno)));
	}

#ifdef WIN32
	sock = my_open_osfhandle(netsock, _O_RDWR | _O_BINARY);
	if (sock < 0) {
	    int er = errno;
	    close();
	    throw runtime_error("my_open_osfhandle: " + string(strerror(er)));
	}
#endif

	stream = fdopen(sock, "rb+");
	if (!stream) {
	    int er = errno;
	    close();
	    throw runtime_error("fdopen: " + string(strerror(er)));
	}
	setvbuf(stream,NULL,_IONBF,0); // no buffering
    }

    void resolve(const string& hostname, const string& service, vector<sockaddr_uni> &addrs) {
	struct addrinfo *ai,*aip,hints;
	int ret;
	
	sockaddr_uni rname;

	memset(&hints,0,sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = PF_UNSPEC;

	ret = getaddrinfo(hostname.length()?hostname.c_str():0,
		service.length()?service.c_str():0, &hints, &ai);
	if (ret) {
	    throw runtime_error("getaddrinfo: " + string(gai_strerror(ret)));
	}
	aip = ai;

	while (aip) {
	    memset(&rname,0,sizeof(rname));
	    memcpy(&rname,aip->ai_addr,aip->ai_addrlen);
	    addrs.push_back(rname);

	    aip = aip->ai_next;
	}

	freeaddrinfo(ai);
    }

    TomiTCP::~TomiTCP()
    {
	close();
    }

    void TomiTCP::close()
    {
#ifdef WIN32
	if (my_close && sock >= 0)
	    my_close(sock);
#endif

	if (stream) {
	    if (fclose(stream))
		cerr << "fclose: " + string(strerror(errno)) << endl;
	    stream = 0;
	    sock = -1;
	}

	if (sock >= 0) {
	    if (::close(sock))
		cerr << "close: " + string(strerror(errno)) << endl;
	    sock = -1;
	}

#ifdef WIN32
	if (netsock >= 0) {
	    if (closesocket(netsock))
		cerr << "closesocket: " + string(strerror(sock_errno)) << endl;
	    netsock = -1;
	}
#endif
    }

    int TomiTCP::nodelay()
    {
	int set_opt = 1;
	int retval = ::setsockopt(netsock,IPPROTO_TCP,TCP_NODELAY,(char*)&set_opt,sizeof(set_opt));
	if (retval)
	    cerr << "Could not set TCP_NODELAY" << endl;
	return retval;
    }

    TomiTCP* TomiTCP::accept()
    {
	TomiTCP *ret = new TomiTCP;

	memcpy(&ret->lname,&lname,sizeof(lname));
	memset(&ret->rname,0,sizeof(ret->rname));
	ret->rname.sa.sa_family = lname.sa.sa_family;

	socklen_t len = SIZEOF_SOCKADDR(ret->rname);
	ret->netsock = TEMP_FAILURE_RETRY(::accept(netsock,&ret->rname.sa,&len));
	if (ret->netsock < 0)
	    throw runtime_error("accept: " + string(strerror(sock_errno)));

#ifdef WIN32
	ret->sock = my_open_osfhandle(ret->netsock, _O_RDWR | _O_BINARY);
	if (ret->sock < 0) {
	    int er = errno;
	    delete ret;
	    throw runtime_error("my_open_osfhandle: " + string(strerror(er)));
	}
#endif

	ret->stream = fdopen(ret->sock,"rb+");
	if (!ret->stream) {
	    int er = errno;
	    delete ret;
	    throw runtime_error("fdopen: " + string(strerror(er)));
	}
	setvbuf(ret->stream,NULL,_IONBF,0); // no buffering
	
	return ret;
    }

    int TomiTCP::getline(string& s, char delim)
    {
	char c;
	int ret;

	s.clear();

	millitime_t start = millitime(), towait = -1;
	while (recv_timeout == 0 ||
		(towait = (millitime_t)recv_timeout * 1000 + start - millitime()) > 0) {
	    ret = input_timeout(netsock, towait);
	    if (ret < 0)
		runtime_error("input_timeout: " + string(strerror(sock_errno)));
	    if (ret > 0) {
		ret = TEMP_FAILURE_RETRY(read(sock, &c, 1));
		if (ret == -1)
		    throw runtime_error("read: " + string(strerror(errno)));
		if (ret == 0)
		    return s.length() > 0;
		if (c == delim)
		    return 1;
		s += c;
	    }
	}

	throw timeout("recv timeout");
	return 0;
    }

    void TomiTCP::set_nonblock_flag(unsigned long value)
    {
#ifdef WIN32
	if (ioctlsocket(netsock, FIONBIO, &value) != 0)
	    throw runtime_error(strerror(sock_errno));
#else
	int oldflags = fcntl(sock, F_GETFL, 0);
	if (oldflags == -1)
	    throw runtime_error(strerror(errno));
	
	if (value != 0)
	    oldflags |= O_NONBLOCK;
	else
	    oldflags &= ~O_NONBLOCK;

	if (fcntl(netsock, F_SETFL, oldflags) == -1)
	    throw runtime_error(strerror(errno));
#endif
    }
    
    string tomi_ntop(const sockaddr_uni& name)
    {
        if (name.sa.sa_family == 0)
            return "";

	char tmp[128],*p;

#ifdef WIN32
	DWORD len = 128;
	sockaddr_uni name2 = name;
	PORT_SOCKADDR(name2) = 0;
	if (WSAAddressToString((struct sockaddr*)&name2.sa,SIZEOF_SOCKADDR(name2),0,tmp,&len))
	    throw runtime_error("WSAAddressToString: " + string(strerror(sock_errno)));
	tmp[len] = 0;
#else
	if (!inet_ntop(name.sa.sa_family,(name.sa.sa_family == AF_INET)?
		    ((const void *)&name.sin.sin_addr):
		    ((const void *)&name.sin6.sin6_addr),tmp,128))
	    throw runtime_error("inet_ntop: " + string(strerror(sock_errno)));
#endif

	if (name.sa.sa_family == AF_INET6 && IN6_IS_ADDR_V4MAPPED(&name.sin6.sin6_addr))
	    if ((p = strstr(tmp,":ffff:")) || (p = strstr(tmp, ":FFFF:")))
		return string(p+6);

	return string(tmp);
    }
    
    void tomi_pton(string p, sockaddr_uni& name)
    {
	if (name.sa.sa_family == AF_INET6 && p.length() &&
		p.find_first_of(':') == string::npos)
	    name.sa.sa_family = AF_INET;
#ifdef WIN32
	int sz = SIZEOF_SOCKADDR(name);
	if (WSAStringToAddress((char*)p.c_str(),name.sa.sa_family,0,(struct sockaddr*)&name.sa,&sz))
	    throw runtime_error("WSAStringToAddress: " + string(strerror(sock_errno)));
#else
	int ret = inet_pton(name.sa.sa_family,p.c_str(),(name.sa.sa_family == AF_INET)?
		((void *)&name.sin.sin_addr):
		((void *)&name.sin6.sin6_addr));
	if (ret < 0)
	    throw runtime_error("inet_pton: " + string(strerror(sock_errno)));
	if (ret == 0)
	    throw runtime_error("Not a valid address");
#endif
    }
    
    string revers(const sockaddr_uni& name)
    {
	char hostname[NI_MAXHOST];
	sockaddr_uni n;
	memset(&n,0,sizeof(name));

	if (name.sa.sa_family == AF_INET6 && IN6_IS_ADDR_V4MAPPED(&name.sin6.sin6_addr)) {
	    n.sa.sa_family = AF_INET;
	    tomi_pton(tomi_ntop(name),n);
	} else {
	    n = name;
	}

	if (getnameinfo((const sockaddr*)&n,SIZEOF_SOCKADDR(name),hostname,sizeof(hostname),0,0,NI_NAMEREQD))
	    return tomi_ntop(name);
	return string(hostname);
    }

    int input_timeout(int filedes, millitime_t ms)
    {
	fd_set set;
        struct timeval timeout, *to = 0;

        FD_ZERO(&set);
        FD_SET(filedes, &set);

	if (ms >= 0) {
	    timeout.tv_sec = ms / 1000;
	    timeout.tv_usec = (ms % 1000) * 1000;
	    to = &timeout;
	}
        return TEMP_FAILURE_RETRY(select(filedes + 1, &set, 0, 0, to));
    }

    int output_timeout(int filedes, millitime_t ms)
    {
	fd_set set;
        struct timeval timeout, *to = 0;

        FD_ZERO(&set);
        FD_SET(filedes, &set);

	if (ms >= 0) {
	    timeout.tv_sec = ms / 1000;
	    timeout.tv_usec = (ms % 1000) * 1000;
	    to = &timeout;
	}
        return TEMP_FAILURE_RETRY(select(filedes + 1, 0, &set, 0, to));
    }
    
    int connect_timeout(int filedes, millitime_t ms)
    {
	fd_set wset, eset;
        struct timeval timeout, *to = 0;

        FD_ZERO(&wset);
        FD_SET(filedes, &wset);
        FD_ZERO(&eset);
        FD_SET(filedes, &eset);

	if (ms >= 0) {
	    timeout.tv_sec = ms / 1000;
	    timeout.tv_usec = (ms % 1000) * 1000;
	    to = &timeout;
	}
        return TEMP_FAILURE_RETRY(select(filedes + 1, 0, &wset, &eset, to));
    }

    millitime_t millitime()
    {
#ifdef WIN32
	struct _timeb tb;
	_ftime(&tb);
	return (millitime_t)tb.time * 1000 + tb.millitm;
#else
	struct timeval tv;
	if (gettimeofday(&tv, 0))
	    throw runtime_error("gettimeofday: " + string(strerror(errno)));
	return (millitime_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
    }

    bool operator == (const sockaddr_uni &a, const sockaddr_uni &b)
    {
        if (a.sa.sa_family != b.sa.sa_family)
            return 0;

        return !memcmp(&a, &b, SIZEOF_SOCKADDR(a));
    }
}
