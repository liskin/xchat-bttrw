/*#include "http.h"
#include "cookies.h"*/
#include <iostream>
#include "net.h"
#include "str.h"
#include <unistd.h>
using namespace std;

int main(int argc, char *argv[])
{
    //if (argc!=2) return -1;

    try {
	/*net::TomiCookies c;
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
	}*/
	/*string s;

	cout << getpid() << endl;

	net::TomiTCP t(1025);

	getline(cin,s);

	t.close();

	getline(cin,s);*/
	/*net::TomiTCP a;
	a.connect("nomi.cz","ircd");
	string s;
	getline(cin,s);*/
	//cout << a.ident() << endl;
	
	//cout << recode("Tomáš Janoušek","utf-8","iso-8859-2") << endl;
    } catch (runtime_error e) {
	cerr << e.what() << endl;
    }
    //cout << net::TomiHTTP::URLencode("ahoj tome") << endl;


    /*cout << reformat(
"   So we know who won rnd, who was send etc.  We use a very simple scoring,\
 nooooooo points for three places, nothing difficult.  For a win, you get 1 point.\
 Simple.  But you can win, win and still win and nobody would say that you're\
 still winning.  So for three wins in a line you get HATRICK.  With HATRICK\
 you get additional 3 points. And if you win 7 times more (10 times at all),\
 you get EXTRA HATRICK and 10 point bonus.  These bonuses are written with big\
 bold font in the middle of field with your color so everybody knows that you\
 just got HATRICK.  Only for having a complete description for this, after you\
 got HATRICK, you have 6 points in 3 rounds (+3p) and after getting EXTRA\
 HATRICK you have 23 points in 10 rounds (+3p +10p).  If you'r still winning,\
 nothinggg will happen.  So if you want more points, it is good to lose one\
 round and then get EXTRA HATRICK and repeat it over and over. If bash is\
 invoked with the name sh, it tries to mimic the startup behavior of\
 historical versions  of  sh  as  closely as possible, while conforming to the\
 POSIX standard as well.  When invoked as an interactive login shell, or a\
 non-interactive shell  with  the  --login option,  it  first attempts to read\
 and execute commands from /etc/profile and ~/.profile, in that order.  The\
 --noprofile option may be used to inhibit this behavior.  When invoked as  an\
 interactive  shell  with the name sh, bash looks for the variable ENV, expands\
 its value if it is defined, and uses the expanded value as the name of a file\
 to read and execute.   Since a shell invoked as sh does not attempt to read\
 and execute commands from any other startup files, the --rcfile option has no\
 effect.  A non-interactive  shell  invoked with  the  name  sh does not\
 attempt to read any other startup files.  When invoked as sh, bash enters\
 posix mode after the startup files are read."
 ,40,"  -  ",8,1,4," - - ");*/
}
