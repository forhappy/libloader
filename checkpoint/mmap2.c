
#include "syscalls.h"
int SCOPE
post_mmap2(struct syscall_regs * regs)
{
	write_syscall_nr(__NR_mmap2);
	write_eax(regs);
	return 0;
}

