#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include "irc.h"
using namespace std;

static int interval = 60;
static int last = 0;

static const char *vowels[] = {
    "a", "e", "i", "o", "u", "y",
    "au", "ou", "eu",
    "l", "r"
};
static int vowels_sz = sizeof(vowels) / sizeof(*vowels);
static const char *consonants[] = {
    "b", "c", "d", "f", "g", "h", "j", "k", "l", "m",
    "n", "p", "q", "r", "s", "t", "v", "w", "x", "z"
};
static int consonants_sz = sizeof(consonants) / sizeof(*consonants);

static string generate_some_shit()
{
    string tmp;

    for (int words = 2 + rand() % 5; words; words--) {
	for (int syllables = 1 + rand() % 4; syllables; syllables--) {
	    if (rand() & 1) {
		tmp += vowels[rand() % vowels_sz];
		tmp += consonants[rand() % consonants_sz];
	    } else {
		tmp += consonants[rand() % consonants_sz];
		tmp += vowels[rand() % vowels_sz];
	    }
	}

	if (words != 1)
	    tmp += ' ';
    }

    return tmp;
}

extern "C" {

void m_talk_timer()
{
    if (time(0) > last + interval) {
	last = time(0);

	if (!channels.size())
	    return;

	int ch = rand() % channels.size();
	channels_t::iterator i = channels.begin();
	for (; ch && i != channels.end(); i++, ch--);

	if (i != channels.end())
	    S(f,"PRIVMSG %s :%s\n", i->first.c_str(), generate_some_shit().c_str());
    }
}

int m_talk_config(string a, string b)
{
    if (!strcasecmp(a.c_str(),"talk_interval"))
        interval = atol(b.c_str());
    else
        return 1;

    return 0;
}

void m_talk_init(struct module &m)
{
    cout << "m_talk init - hello" << endl;
    try {
	m.timer = m_talk_timer;
	m.config = m_talk_config;
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }
}

}
