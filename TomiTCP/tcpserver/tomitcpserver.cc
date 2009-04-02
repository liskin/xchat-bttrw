#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "../net.h"
using namespace std;

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
	    pid_t p = fork(); if (p < 0) throw runtime_error(strerror(errno));
	    if (!p) {
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
