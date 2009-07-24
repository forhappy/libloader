
#include "syscalls.h"

#ifdef IN_INJECTOR
# include "injector.h"
#endif

#ifndef SYSCALL_PRINTER
int SCOPE
post_mmap2(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

int SCOPE
replay_mmap2(const struct syscall_regs * regs)
{
	uint32_t eax;
	eax = read_uint32();
	if (eax != 0xffffffffUL) {
		/* we must do the mmap for target */
#ifdef IN_INJECTOR
		uint32_t ret;
		ret = INTERNAL_SYSCALL(mmap2, 6,
				regs->ebx, regs->ecx, regs->edx,
				regs->esi, regs->edi, regs->ebp);
		ASSERT(ret == eax, "");
#endif
	}
	return eax;
}


#else

void
output_mmap2(void)
{
	printf("mmap2:\t0x%x\n", read_eax());
}
#endif

