#pragma implementation
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include "net.h"
#ifndef WIN32
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/tcp.h>
# include <netdb.h>
#else
# include <io.h>
# include <fcntl.h>
# include <mswsock.h>
#endif
#include "str.h"

#ifndef WIN32
# define sock_errno errno
#else
# define sock_errno WSAGetLastError()
# define EAFNOSUPPORT WSAEAFNOSUPPORT
# define TEMP_FAILURE_RETRY(a) (a)
const char * wsock_strerror(int err);
# define strerror wsock_strerror
# undef gai_strerror
# define gai_strerror strerror
int winsock_init() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 0 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
	std::cerr << "Too old winsock" << std::endl;
	throw 0;
    }

    int iSockOpt = SO_SYNCHRONOUS_NONALERT;
    setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE,
	    (char *)&iSockOpt, sizeof(iSockOpt));

    return 0;
}
int winsock_init_tmp = winsock_init();
#endif

namespace net {
    TomiTCP::TomiTCP() : sock(-1), stream(0)
#ifdef WIN32
			 , w32socket(-1), netsock(w32socket)
#else
			 , netsock(sock)
#endif
    {
	memset(&lname,0,sizeof(lname));
	memset(&rname,0,sizeof(rname));
    }

    TomiTCP::TomiTCP(uint16_t port, const string &addr) : sock(-1), stream(0)
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
	if (ok()) {
	    close();
	}

	memset(&lname,0,sizeof(lname));
	lname.sa.sa_family = AF_INET6;
	tomi_pton(addr,lname);
	PORT_SOCKADDR(lname) = htons(port);

	sock = ::socket(lname.sa.sa_family, SOCK_STREAM, 0);
	if (sock < 0 && (sock_errno == EINVAL || sock_errno == EAFNOSUPPORT) && addr == "::") {
                lname.sin.sin_family = AF_INET;
                lname.sin.sin_addr.s_addr = INADDR_ANY;
                
                sock = ::socket(PF_INET, SOCK_STREAM, 0);
        } else if (sock < 0) {
	    throw runtime_error(string(strerror(sock_errno)));
	}

	int set_opt = 1;
        if (::setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&set_opt,sizeof(set_opt)))
	    cerr << "Could not set SO_REUSEADDR" << endl;

	if (::bind(sock,&lname.sa,SIZEOF_SOCKADDR(lname))) {
	    int er = sock_errno;
	    ::close(sock);
	    sock = -1;
	    throw runtime_error(string(strerror(er)));
	}

	if (::listen(sock,5)) {
	    int er = sock_errno;
	    ::close(sock);
	    sock = -1;
	    throw runtime_error(string(strerror(er)));
	}

#ifdef WIN32
	w32socket = sock;
#endif
    }

    TomiTCP::TomiTCP(const string& hostname, uint16_t port) : sock(-1), stream(0)
#ifdef WIN32
		     , w32socket(-1), netsock(w32socket)
#else
		     , netsock(sock)
#endif
    {
	memset(&lname,0,sizeof(lname));
	memset(&rname,0,sizeof(rname));
	connect(hostname,uinttostr(port));
    }

    void TomiTCP::connect(const string& hostname, const string& service)
    {
	if (ok()) {
	    close();
	}

	vector<sockaddr_uni> addrs;
	resolve(hostname,service,addrs);

	string err = "No adress to connect to";

	for (vector<sockaddr_uni>::iterator i = addrs.begin(); i != addrs.end(); i++) {
	    memcpy(&rname,&(*i),SIZEOF_SOCKADDR(*i));

	    sock = ::socket(rname.sa.sa_family, SOCK_STREAM, 0);
	    if (socket < 0) {
		err = strerror(sock_errno);
	    } else {
		if (::connect(sock,(const sockaddr*)&rname,SIZEOF_SOCKADDR(rname))) {
		    int er = sock_errno;
		    close();
		    err = strerror(er);
		} else {
		    break;
		}
	    }
	}

	if (!ok())
	    throw (runtime_error(err));

	socklen_t len = SIZEOF_SOCKADDR(lname);
	if (getsockname(sock,(sockaddr*)&lname,&len)) {
	    throw runtime_error(string(strerror(sock_errno)));
	}

#ifdef WIN32
	w32socket = sock;
	sock = _open_osfhandle(sock, _O_RDWR | _O_BINARY);
#endif

	stream = fdopen(sock, "rb+");
	if (!stream) {
	    int er = errno;
	    close();
	    throw runtime_error(string(strerror(er)));
	}
	setvbuf(stream,NULL,_IONBF,0); // no buffering
    }

    void resolve(const string& hostname, const string& service, vector<sockaddr_uni> &addrs) {
	struct addrinfo *ai,*aip,hints;
	int ret;
	
	sockaddr_uni rname;

	memset(&hints,0,sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(hostname.c_str(),service.c_str(),&hints,&ai);
	if (ret) {
	    throw runtime_error(string(gai_strerror(ret)));
	}
	aip = ai;

	string err = "No adress for hostname";

	while (aip) {
	    memset(&rname,0,sizeof(rname));
	    memcpy(&rname,aip->ai_addr,aip->ai_addrlen);
	    addrs.push_back(rname);

	    aip = aip->ai_next;
	}

	freeaddrinfo(ai);
    }

    string TomiTCP::ident(int ms)
    {
	sockaddr_uni addr = rname;
	PORT_SOCKADDR(addr) = htons(113);
	int s = ::socket(addr.sa.sa_family, SOCK_STREAM, 0);
	if (s < 0) {
	    throw runtime_error(string("Ident: ")+strerror(sock_errno));
	}
	if (::connect(s,(const sockaddr*)&addr,SIZEOF_SOCKADDR(addr))) {
	    int er = sock_errno;
	    ::close(s);
	    throw runtime_error(string("Ident: ")+strerror(er));
	}

	char query[64];
	sprintf(query,"%hi , %hi\n",ntohs(PORT_SOCKADDR(rname)),ntohs(PORT_SOCKADDR(lname)));

	int retval = TEMP_FAILURE_RETRY(::send(s,query,strlen(query),0));
	if (retval < 0) {
	    int er = sock_errno;
	    ::close(s);
	    throw runtime_error(string("Ident: ")+string(strerror(er)));
	}

	char buffer[513];

	if (input_timeout(s,ms) > 0)
	{
	    retval = TEMP_FAILURE_RETRY(::recv(s,buffer,512,0));
	    if (retval <= 0) {
		int er = sock_errno;
		::close(s);
		if (retval < 0)
		    throw runtime_error(string("Ident: ")+string(strerror(er)));
		return "";
	    }
	    buffer[retval] = 0;
	} else {
	    throw timeout("ident Timeout (recv)");
	}
	::close(s);

	stringstream ss(buffer);
	string tmp;

	std::getline(ss,tmp,':');
	ss >> tmp;
	if (tmp != "USERID") {
	    return "";
	} else {
	    std::getline(ss,tmp,':');
	    std::getline(ss,tmp,':');
	    std::getline(ss,tmp,' ');
	    std::getline(ss,tmp);
	    return tmp;
	}
    }

    /*
     * This takes a "unix file descriptor" as an argument, on Win32, you have
     * to call _open_osfhandle to get it.
     */
    void TomiTCP::attach(int filedes)
    {
	sock = dup(filedes);
	if (sock < 0)
	    throw runtime_error(string(strerror(errno)));

	stream = fdopen(sock,"rb+");
	if (!stream) {
	    int er = errno;
	    close();
	    throw runtime_error(string(strerror(er)));
	}

	// fill lname & rname !!!

	setvbuf(stream,NULL,_IONBF,0); // no buffering
    }

    TomiTCP::~TomiTCP()
    {
	close();
    }

    void TomiTCP::close()
    {
	if (stream) {
	    if (fclose(stream))
		throw runtime_error(string(strerror(errno)));
	    stream = 0;
	    sock = -1;
	} else if (sock >= 0) {
	    ::close(sock);
	    sock = -1;
	}
#ifdef WIN32
	if (w32socket >= 0)
	    closesocket(w32socket);
#endif
    }

    bool TomiTCP::ok()
    {
	return (sock >= 0);
    }

    int TomiTCP::nodelay()
    {
	int set_opt = 1;
	int retval = ::setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char*)&set_opt,sizeof(set_opt));
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
	ret->sock = TEMP_FAILURE_RETRY(::accept(sock,&ret->rname.sa,&len));
	if (ret->sock < 0)
	    throw runtime_error(string(strerror(sock_errno)));

#ifdef WIN32
	ret->w32socket = ret->sock;
	ret->sock = _open_osfhandle(ret->sock, _O_RDWR | _O_BINARY);
#endif

	ret->stream = fdopen(ret->sock,"rb+");
	if (!ret->stream) {
	    int er = errno;
	    delete ret;
	    throw runtime_error(string(strerror(er)));
	}
	setvbuf(ret->stream,NULL,_IONBF,0); // no buffering
	
	return ret;
    }

    /*
     * This is considered deprecated and is subject to remove during next
     * cleanup. Use operator FILE* instead.
     */
    FILE* TomiTCP::makestream()
    {
	int s = dup(sock);
	if (s < 0)
	    throw runtime_error(string(strerror(errno)));

	FILE *f = fdopen(s,"rb+");
	if (!f)
	    throw runtime_error(string(strerror(errno)));
	setvbuf(f,NULL,_IONBF,0); // no buffering

	return f;
    }

    int TomiTCP::getline(string& s, char delim)
    {
	char c;
	int ret;

	s.clear();
	while ((ret = read(sock, &c, 1)) == 1 && c != delim) {
	    s += c;
	}

	if (ret == -1)
	    throw runtime_error(strerror(errno));

	return (ret != 0) || (s.length() > 0);
    }
    
    string tomi_ntop(const sockaddr_uni& name)
    {
	char tmp[128],*p;

#ifdef WIN32
	DWORD len = 128;
	if (WSAAddressToString((struct sockaddr*)&name.sa,SIZEOF_SOCKADDR(name),0,tmp,&len))
	    throw runtime_error(string(strerror(sock_errno)));
	tmp[len] = 0;
#else
	if (!inet_ntop(name.sa.sa_family,(name.sa.sa_family == AF_INET)?
		    ((const void *)&name.sin.sin_addr):
		    ((const void *)&name.sin6.sin6_addr),tmp,128))
	    throw runtime_error(string(strerror(sock_errno)));
#endif

	if (name.sa.sa_family == AF_INET6 && IN6_IS_ADDR_V4MAPPED(&name.sin6.sin6_addr))
	    if ((p = strstr(tmp,":ffff:")) || (p = strstr(tmp, ":FFFF:")))
		return string(p+6);

	return string(tmp);
    }
    
    void tomi_pton(string p, sockaddr_uni& name)
    {
	if (name.sa.sa_family == AF_INET6 && p.length() && ((int)p.find_first_of(':'))==-1)
	    name.sa.sa_family = AF_INET;
#ifdef WIN32
	int sz = SIZEOF_SOCKADDR(name);
	if (WSAStringToAddress((char*)p.c_str(),name.sa.sa_family,0,(struct sockaddr*)&name.sa,&sz))
	    throw runtime_error(string(strerror(sock_errno)));
#else
	int ret = inet_pton(name.sa.sa_family,p.c_str(),(name.sa.sa_family == AF_INET)?
		((void *)&name.sin.sin_addr):
		((void *)&name.sin6.sin6_addr));
	if (ret < 0)
	    throw runtime_error(string(strerror(sock_errno)));
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

    int input_timeout(int filedes, int ms)
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
        return TEMP_FAILURE_RETRY(select(FD_SETSIZE,&set,NULL,NULL,to));
    }

    int output_timeout(int filedes, int ms)
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
        return TEMP_FAILURE_RETRY(select(FD_SETSIZE,NULL,&set,NULL,to));
    }

#ifdef WIN32
#undef strerror
    const char * wsock_strerror(int err) {
	const char *error = 0;

	switch(err) {
	    case 10004: error = "Interrupted system call"; break;
	    case 10009: error = "Bad file number"; break;
	    case 10013: error = "Permission denied"; break;
	    case 10014: error = "Bad address"; break;
	    case 10022: error = "Invalid argument (not bind)"; break;
	    case 10024: error = "Too many open files"; break;
	    case 10035: error = "Operation would block"; break;
	    case 10036: error = "Operation now in progress"; break;
	    case 10037: error = "Operation already in progress"; break;
	    case 10038: error = "Socket operation on non-socket"; break;
	    case 10039: error = "Destination address required"; break;
	    case 10040: error = "Message too long"; break;
	    case 10041: error = "Protocol wrong type for socket"; break;
	    case 10042: error = "Bad protocol option"; break;
	    case 10043: error = "Protocol not supported"; break;
	    case 10044: error = "Socket type not supported"; break;
	    case 10045: error = "Operation not supported on socket"; break;
	    case 10046: error = "Protocol family not supported"; break;
	    case 10047: error = "Address family not supported by protocol family"; break;
	    case 10048: error = "Address already in use"; break;
	    case 10049: error = "Can't assign requested address"; break;
	    case 10050: error = "Network is down"; break;
	    case 10051: error = "Network is unreachable"; break;
	    case 10052: error = "Net dropped connection or reset"; break;
	    case 10053: error = "Software caused connection abort"; break;
	    case 10054: error = "Connection reset by peer"; break;
	    case 10055: error = "No buffer space available"; break;
	    case 10056: error = "Socket is already connected"; break;
	    case 10057: error = "Socket is not connected"; break;
	    case 10058: error = "Can't send after socket shutdown"; break;
	    case 10059: error = "Too many references, can't splice"; break;
	    case 10060: error = "Connection timed out"; break;
	    case 10061: error = "Connection refused"; break;
	    case 10062: error = "Too many levels of symbolic links"; break;
	    case 10063: error = "File name too long"; break;
	    case 10064: error = "Host is down"; break;
	    case 10065: error = "No Route to Host"; break;
	    case 10066: error = "Directory not empty"; break;
	    case 10067: error = "Too many processes"; break;
	    case 10068: error = "Too many users"; break;
	    case 10069: error = "Disc Quota Exceeded"; break;
	    case 10070: error = "Stale NFS file handle"; break;
	    case 10091: error = "Network SubSystem is unavailable"; break;
	    case 10092: error = "WINSOCK DLL Version out of range"; break;
	    case 10093: error = "Successful WSASTARTUP not yet performed"; break;
	    case 10071: error = "Too many levels of remote in path"; break;
	    case 11001: error = "Host not found"; break;
	    case 11002: error = "Non-Authoritative Host not found"; break;
	    case 11003: error = "Non-Recoverable errors: FORMERR, REFUSED, NOTIMP"; break;
	    case 11004: error = "Valid name, no data record of requested type"; break;
	    default: error = strerror(err); break;
	}

	if (!error || !*error) {
	    static char t[20];
	    sprintf(t, "%i", err);
	    error = t;
	}

	return error;
    }
#endif
}
