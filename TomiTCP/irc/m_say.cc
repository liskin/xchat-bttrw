#include <iostream>
#include <vector>
#include <string>
#include "irc.h"
using namespace std;

extern "C" {

int m_say_port = 12204;
net::TomiTCP m_say_s;
bool giveup = 0;

void m_say_timer()
{
    if (giveup)
	return;
    try {
	if (!m_say_s.ok()) {
	    m_say_s.listen(m_say_port);
	}
	if (m_say_s.ok() && net::input_timeout(m_say_s.sock,0) > 0) {
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
	cerr << "m_say: " << e.what() << endl;
	giveup = 1;
    }
}

int m_say_config(string a, string b)
{
    if (!strcasecmp(a.c_str(),"say_port"))
        m_say_port = atol(b.c_str());
    else
        return 1;

    return 0;
}

void m_say_init(struct module &m)
{
    cout << "m_say init - hello" << endl;
    try {
	m.timer = m_say_timer;
	m.config = m_say_config;
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }
}

}
