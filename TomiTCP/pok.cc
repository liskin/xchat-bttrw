#include <iostream>
#include "net.h"
using namespace std;

int main(int argc, char *argv[])
{
    iostream *s = 0;
    string a;
    try {
	/*net::sockaddr_uni a;
	a.sa.sa_family = AF_INET6;
	net::tomi_pton("::fFff:80.83.66.17",a);
	std::cerr << net::tomi_ntop(a) << std::endl;
	std::cerr << net::revers(a) << std::endl;
	return 0;*/
	net::TomiTCP sock(12189);
	net::TomiTCP& client = *(sock.accept());

	std::cerr << net::tomi_ntop(client.name) << std::endl;

	client.send("bagr\n",5);

	char buf[50];
	try {
	    int ret = client.recv(buf,50);
	    if (ret) {
		buf[ret] = 0;
		std::cerr << buf << std::endl;
	    } else {
		std::cerr << "Zavrel spojeni" << std::endl;
	    }
	} catch (net::timeout e) {
	    std::cerr << "TO: " << e.what() << std::endl;
	}
	/*net::TomiTCP sock("nomi.cz",80);
	s = sock.makestream();
	*s << "HEAD / HTTP/1.0\n\n";
	while (getline(*s,a)) {
	    cout << a << endl;
	}
	delete s; s = 0;*/

	/*sock.send("HEAD / HTTP/1.0\n\n",17);
	char buf[51];
	while (int ret = sock.recv(buf,50)) {
	    buf[ret] = 0;
	    std::cout << buf;
	}*/
    } catch (std::runtime_error e) {
	std::cerr << e.what() << std::endl;
	if (s)
	    delete s;
    }
    return 0;
}
