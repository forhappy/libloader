#include "syscalls.h"

struct iovec
{
	void *iov_base;	/* BSD uses caddr_t (1003.1g requires void *) */
	__kernel_size_t iov_len; /* Must be size_t (1003.1g) */
};

#ifndef SYSCALL_PRINTER

int SCOPE
post_vmsplice(const struct syscall_regs * regs)
{
	/* i am vmsplice, i don't need to write the content. */
	write_eax(regs);
	return 0;
}

int SCOPE
replay_vmsplice(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	return eax;
}

#else

void
output_vmsplice(int nr)
{
	int32_t ret = read_eax();
	printf("vmsplice:\t%d\n", ret);
}
#endif

