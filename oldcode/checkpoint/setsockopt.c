
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_setsockopt(int fd, int level, int optname,
		uint32_t optval, int optlen, int retval, const struct syscall_regs * regs)
{
	return 0;
}

int SCOPE
replay_setsockopt(int fd, int level, int optname,
		uint32_t optval, int optlen, int retval, const struct syscall_regs * regs)
{
	return retval;
}


#else

void SCOPE
output_setsockopt(int fd, int level, int optname,
		uint32_t optval, int optlen, int retval)
{
	printf("seteockopt(fd=%d, level=%d, optname=%d, optval=0x%x, optlen=%d):\t%d\n",
			fd, level, optname, optval, optlen, retval);
}

#endif

