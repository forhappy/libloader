#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_sendfile64(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_sendfile64(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	return eax;
}

#else

void
output_sendfile64(void)
{
	int32_t ret = read_eax();
	printf("sendfile64:\t%d\n", ret);
}
#endif

