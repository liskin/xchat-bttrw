#include <iostream>
#include <stdexcept>
#include "xchat.h"
using namespace std;
using namespace xchat;

int main(int argc, char *argv[])
{
    if (argc != 3) {
	cerr << argv[0] << " <user> <pass>" << endl;
	return -1;
    }

    try {
	XChat x(argv[1],argv[2]);
	cout << "Logged in uid=" << x.uid << " sid=" << x.sid << endl;
	int l = x.join("324979");
	cout << "DDD " << l << endl;
	while (1) {
	    cout << "---------------" << endl;
	    vector<string> m;
	    l = x.getmsg("324979",l,m);
	    for (vector<string>::iterator i = m.begin(); i != m.end(); i++) {
		cout << "DD= " << *i << endl;
	    }
	    cout << "DDD " << l << endl;
	    sleep(5);
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}
