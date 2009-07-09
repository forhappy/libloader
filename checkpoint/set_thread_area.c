
#include "syscalls.h"

#ifndef SYSCALL_PRINTER

int SCOPE
post_set_thread_area(struct syscall_regs * regs)
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

#else

void
output_set_thread_area(void)
{
	printf("set_thread_area:\t%d\n", read_eax());
}
#endif

