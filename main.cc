#include <iostream>
#include <sstream>
#include <stdexcept>
#include "xchat.h"
#include "TomiTCP/net.h"
using namespace std;
using namespace xchat;

string striphtml(string a)
{
    string out;
    stringstream s(a);

    while (getline(s,a,'<')) {
	out += a + " ";
	getline(s,a,'>');
    }

    return out;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
	cerr << argv[0] << " <user> <pass> <channel>" << endl;
	return -1;
    }

    string room = argv[3];

    try {
	XChat x(argv[1],argv[2]);
	cout << "Logged in uid=" << x.uid << " sid=" << x.sid << endl;

	int l = x.join(room);

	while (1) {
	    vector<string> m;
	    l = x.getmsg(room,l,m);
	    for (vector<string>::iterator i = m.begin(); i != m.end(); i++) {
		cout << striphtml(*i) << endl;
	    }

	    sleep(5);

	    if (net::input_timeout(0,0) > 0) {
		string tmp;
		getline(cin,tmp);
		if (!tmp.length())
		    break;
		x.putmsg(room,tmp);
	    }
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}
