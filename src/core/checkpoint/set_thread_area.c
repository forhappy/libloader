
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_set_thread_area(const struct syscall_regs * regs)
{
	
	write_eax(regs);
	if (regs->eax >= 0) {
		struct user_desc desc;
		__dup_mem(&desc, regs->ebx, sizeof(desc));
		/* set state vector */
		state_vector.thread_area[desc.entry_number - GDT_ENTRY_TLS_MIN] = desc;
		/* record result */
		write_mem(regs->ebx, sizeof(desc));
	}
	return 0;
}

int SCOPE
replay_set_thread_area(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax >= 0) {
		read_mem(regs->ebx, sizeof(struct user_desc));
#ifdef IN_INJECTOR
		int32_t ret;
		ret = INTERNAL_SYSCALL(set_thread_area, 1,
				regs->ebx);
		ASSERT(ret == eax, regs, "!@#@\n");
#endif
	}
	return eax;
}

#else

void
output_set_thread_area(int nr)
{
	printf("set_thread_area:\t%d\n", read_eax());
}
#endif

