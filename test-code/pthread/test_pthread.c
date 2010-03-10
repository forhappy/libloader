#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <asm/unistd_32.h>

static long int __gettid()
{
	return syscall(__NR_gettid);
}

static long int __tgkill(int x, int y, int z)
{
	return syscall(__NR_tgkill, x, y, z);
}

static long int __kill(int x, int y)
{
	return syscall(__NR_kill, x, y);
}

static long int __exit(int x)
{
	return syscall(__NR_exit, x);
}

static long int __exit_group(int x)
{
	return syscall(__NR_exit_group, x);
}


static void
sighandler(int num)
{
	printf("signal %d, pid=%d, tid=%ld\n", num, getpid(), __gettid());
}

static pthread_key_t tsd_key;

static void * my_thread(void* str)
{
	printf("erron of %ld is %d\n",
			__gettid(), errno);
	pthread_setspecific(tsd_key, str + 1);
	for (int i = 0; i < 10; i++) {
		char * p = pthread_getspecific(tsd_key);
		printf("%s\n", (char*)p);
		sleep(1);
	}
	return NULL;
}

int main()
{

	printf("main: erron of %ld is %d\n",
			__gettid(), errno);

	signal(SIGSTOP, sighandler);
	signal(SIGTSTP, sighandler);
	signal(SIGUSR1, sighandler);
//	signal(SIGINT, sighandler);

	pthread_t th1;
	pthread_t th2;

	pthread_key_create(&tsd_key, NULL);

	int err;
	err = pthread_create(&th1, NULL, my_thread, "str1");
	assert(err == 0);
	err = pthread_create(&th2, NULL, my_thread, "str2");
	assert(err == 0);

	sleep(1);
//	__kill(getpid() + 1, SIGKILL);
//	__tgkill(getpid(), getpid(), SIGUSR1);

	__exit_group(10);

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	printf("finished\n");

	return 0;
}

