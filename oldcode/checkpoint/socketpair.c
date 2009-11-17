
#include "socketcall.h"
#ifndef SYSCALL_PRINTER

int SCOPE
post_socketpair(int family, int type, int protocol, uintptr_t usockvec,
		int retval, const struct syscall_regs * regs)
{
	if (retval >= 0) {
		ASSERT(usockvec != 0, regs, "socketpair inconsistent\n");
		write_mem(usockvec, 2 * sizeof(int));
	}
	return 0;
}

int SCOPE
replay_socketpair(int family, int type, int protocol, uintptr_t usockvec,
		int retval, const struct syscall_regs * regs)
{
	if (retval >= 0)
		read_mem(usockvec, 2 * sizeof(int));
	return retval;
}


#else

void SCOPE
output_socketpair(int family, int type, int protocol, uintptr_t usockvec,
		int retval)
{
	printf("socketpair(family=%d, type=%d, protocol=%d, usockvec=0x%x):\t%d\n",
			family, type, protocol, usockvec, retval);
	if (retval >= 0)
		skip(2 * sizeof(int));
}

#endif

