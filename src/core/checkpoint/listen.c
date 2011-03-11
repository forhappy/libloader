
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_listen(int fd, int backlog, int retval, const struct syscall_regs * regs)
{
	return 0;
}

int SCOPE
replay_listen(int fd, int backlog, int retval, const struct syscall_regs * regs)
{
	return retval;
}


#else

void SCOPE
output_listen(int fd, int backlog, int retval)
{
	printf("listen(fd=%d, backlog=%d):\t%d\n",
			fd, backlog, retval);
}

#endif

