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
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}
