#include "http.h"
#include "cookies.h"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc!=2) return -1;

    /*try {
	net::TomiCookies c;
	try {
	    c.load("cookies");
	} catch (...) {
	}
	net::TomiHTTP s;
	//int ret = s.GET(argv[1],&c);
	int ret = s.POST(argv[1],"ahoj=nazdar&nazdar=bagr",&c);
	cout << ret << endl;
	c.save("cookies");

	string l;
	while (s.getline(l)) {
	    cout << l;
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }*/
    cout << net::TomiHTTP::URLencode("ahoj tome") << endl;
}
