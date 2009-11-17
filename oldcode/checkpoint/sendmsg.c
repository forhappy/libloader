
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_sendmsg(int fd, uintptr_t msg, uint32_t flags,
		int32_t retval, const struct syscall_regs * regs)
{
	return 0;
}

int SCOPE
replay_sendmsg(int fd, uintptr_t msg, uint32_t flags,
		int32_t retval, const struct syscall_regs * regs)
{
	return retval;
}


#else

void SCOPE
output_sendmsg(int fd, uintptr_t msg, uint32_t flags,
		int retval)
{
	printf("sendmsg(fd=%d, msg=0x%x, flags=0x%x):\t%d\n",
			fd, msg, flags, retval);
}

#endif

