#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "../net.h"
using namespace std;

int sendall(int s, const char* buf, const int size)
{
    int sz = size;
    const char *p = buf;
    while (sz) {
	int ret = TEMP_FAILURE_RETRY(write(s,p,sz));
	if (ret == -1) {
	    return -1;
	}
	p += ret;
	sz -= ret;
    }

    return size;
}

void chld(int)
{
    ::wait(0);
}

int main(int argc, char *argv[], char *envp[])
{
    if (argc < 4) {
	cout << "host port program" << endl;
	return -1;
    }

    signal(SIGCHLD, chld);

    daemon(0, 0);

    net::TomiTCP s, *c;
    try {
	s.listen(atol(argv[2]), argv[1]);

	while ((c = s.accept())) {
	    if (!fork()) {
		dup2(c->sock, 0);
		dup2(c->sock, 1);
		dup2(c->sock, 2);

		execve(argv[3], &argv[3], envp);
		return -1;
	    }

	    delete c;
	}
    } catch (runtime_error e) {
	cerr << e.what() << endl;
	return -1;
    }

    return 0;
}
