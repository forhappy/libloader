
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_send(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		int retval, const struct syscall_regs * regs)
{
	return 0;
}

int SCOPE
replay_send(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		int retval, const struct syscall_regs * regs)
{
	return retval;
}


#else

void SCOPE
output_send(int fd, uint32_t buff, uint32_t len, uint32_t flags,
		int retval)
{
	printf("send(fd=%d, buff=0x%x, len=%d, flags=0x%x,):\t%d\n",
			fd, buff, len, flags, retval);
}

#endif

