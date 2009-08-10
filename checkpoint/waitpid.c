
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_waitpid(const struct syscall_regs * regs)
{
	write_eax(regs);
	uintptr_t stat_addr = regs->ecx;
	write_obj(stat_addr);

	if (stat_addr != 0)
		write_mem(stat_addr, sizeof(int));
	return 0;
}

int SCOPE
replay_waitpid(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	uintptr_t stat_addr = read_uint32();
	ASSERT(stat_addr == regs->ecx, "stat_addr inconsistent\n");
	if (stat_addr != 0)
		read_mem(stat_addr, sizeof(int));
	return eax;
}
#else

void
output_waitpid(void)
{
	printf("waitpid:\t%d\n", read_eax());
	uintptr_t stat_addr = read_uint32();
	if (stat_addr != 0)
		skip(sizeof(int));
}
#endif

