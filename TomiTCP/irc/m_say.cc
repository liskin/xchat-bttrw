#include <iostream>
#include <vector>
#include <string>
#include "irc.h"
using namespace std;

extern "C" {

net::TomiTCP m_say_s;

void m_say_timer(FILE *f)
{
    try {
	if (net::input_timeout(m_say_s.sock,0) > 0) {
	    net::TomiTCP *cl = m_say_s.accept();

	    string channel, msg;
	    cl->getline(channel);
	    cl->getline(msg);

	    channel.erase(channel.find_last_not_of("\r\n") + 1);
	    msg.erase(msg.find_last_not_of("\r\n") + 1);

	    if (channel.length() && msg.length()) {
		S(f,"PRIVMSG %s :%s\n",channel.c_str(),msg.c_str());
	    }

	    delete cl;
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }
}

void m_say_init(struct module &m)
{
    cout << "m_say init - hello" << endl;
    try {
	m_say_s.listen(12204);
	m.timer = m_say_timer;
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }
}

}
