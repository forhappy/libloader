#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd = open("/dev/xxx", O_RDONLY);
	printf("%d\n", errno);
	return fd;
}

