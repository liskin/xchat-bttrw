#include "http.h"
#include "cookies.h"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc!=2) return -1;

    try {
	net::TomiCookies c;
	net::TomiHTTP s;
	int ret = s.GET(argv[1],&c);
	cout << ret << endl;
	cout << s.headers["content-type"] << endl;
	string aaa;
	c.http_getcookies(aaa);
	cout << aaa;

	string l;
	while (s.getline(l)) {
	    cout << l;
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }
}
