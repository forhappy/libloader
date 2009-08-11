
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_brk(const struct syscall_regs * regs)
{
	write_eax(regs);
	state_vector.brk = regs->eax;
	return 0;
}

int SCOPE
replay_brk(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	/* here we rerun brk */
#ifdef IN_INJECTOR
	uint32_t err;
	err = INTERNAL_SYSCALL(brk, 1,
			regs->ebx);
	ASSERT((uint32_t)eax == err, regs, "brk inconsiste: retval=0x%x, should be 0x%x\n",
			err, eax);
#endif
	return eax;
}
#else

void
output_brk(void)
{
	printf("brk:\t0x%x\n", read_eax());
}
#endif

