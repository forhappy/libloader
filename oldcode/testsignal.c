#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <memory.h>

void
chld_handler(int signum, struct siginfo * pinfo)
{
	printf("SIGCHLD: chld %d\n", pinfo->si_pid);
	fflush(stdout);
}

#define NR	(200)

int main()
{
	pid_t pids[NR];
	struct sigaction act;
	act.sa_handler = (void (*)(int))chld_handler;
	act.sa_flags = SA_SIGINFO;
	memset(&act.sa_mask, 0, sizeof(act.sa_mask));
	sigaction(SIGCHLD, &act, NULL);

	for (int i = 0; i < NR; i++) {
		pid_t p = fork();
		if (p == 0) {
			usleep(200);
			exit(0);
		} else {
			pids[i] = p;
			printf("chld %d forked\n", p);
		}
	}

	for (int i = 0; i < NR; i++) {
		pid_t p;
		p = waitpid(WAIT_ANY, NULL, 0);
		printf("chld %d return\n", p);
	}

	return 0;
}

