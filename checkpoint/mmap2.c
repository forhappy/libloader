
#include "syscalls.h"
#include <sys/mman.h>
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
	INJ_WARNING("XXXX, eax=0x%x\n", eax);
	INJ_WARNING("XXXX, ebx=0x%x\n", regs->ebx);
	INJ_WARNING("XXXX, ecx=0x%x\n", regs->ecx);
	INJ_WARNING("XXXX, edx=0x%x\n", regs->edx);
	INJ_WARNING("XXXX, esi=0x%x\n", regs->esi);
	INJ_WARNING("XXXX, edi=0x%x\n", regs->edi);
	INJ_WARNING("XXXX, ebp=0x%x\n", regs->ebp);
	if (eax != 0xffffffffUL) {
		INJ_WARNING("Try...\n");
		/* we must do the mmap for target */
#ifdef IN_INJECTOR
		uint32_t ret;
		ret = INTERNAL_SYSCALL(mmap2, 6,
				regs->ebx, regs->ecx, regs->edx,
				regs->esi, regs->edi, regs->ebp);
		INJ_WARNING("XXXX, ret=0x%x\n", ret);
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

