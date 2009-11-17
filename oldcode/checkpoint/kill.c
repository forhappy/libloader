
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_kill(const struct syscall_regs * regs)
{
	write_eax(regs);
	uint32_t pid, sig;
	pid = regs->ebx;
	sig = regs->ecx;
	write_obj(pid);
	write_obj(sig);
	return 0;
}

int SCOPE
replay_kill(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	int32_t pid, sig;
	pid = read_int32();
	sig = read_int32();
	ASSERT(pid == regs->ebx, regs, "kill pid inconsistent\n");
	ASSERT(sig == regs->ecx, regs, "kill sig inconsistent\n");
	return eax;
}
#else

void
output_kill(int nr)
{
	int32_t eax = read_eax();
	int32_t pid, sig;
	pid = read_int32();
	sig = read_int32();
	printf("kill(%d, %d):\t%d\n", pid, sig, eax);
}
#endif

