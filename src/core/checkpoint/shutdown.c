
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_shutdown(int fd, int how, int retval, const struct syscall_regs * regs)
{
	return 0;
}

int SCOPE
replay_shutdown(int fd, int how, int retval, const struct syscall_regs * regs)
{
	return retval;
}


#else

void SCOPE
output_shutdown(int fd, int how, int retval)
{
	printf("shutdown(fd=%d, how=%d):\t%d\n",
			fd, how, retval);
}

#endif

