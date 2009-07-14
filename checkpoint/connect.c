
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_connect(int fd, uint32_t uservaddr, int addrlen, int retval)
{
	return 0;
}

#else

void SCOPE
output_connect(int fd, uint32_t uservaddr, int addrlen, int retval)
{
	printf("connect(fd=%d, uservaddr=0x%x, addrlen=%d):\t%d\n",
			fd, uservaddr, addrlen, retval);
}

#endif

