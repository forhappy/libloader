
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_bind(int fd, uint32_t umyaddr, int addrlen, int retval)
{
	return 0;
}

#else

void SCOPE
output_bind(int fd, uint32_t umyaddr, int addrlen, int retval)
{
	printf("bind(fd=%d, umyaddr=0x%x, addrlen=%d):\t%d\n",
			fd, umyaddr, addrlen, retval);
}

#endif

