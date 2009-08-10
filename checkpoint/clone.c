
#include "syscalls.h"

#define CLONE_PARENT_SETTID     0x00100000      /*  set the TID in the parent */

#ifndef SYSCALL_PRINTER
#ifdef IN_INJECTOR
#include "checkpoint/newprocess.h"
#endif
int SCOPE
post_clone(const struct syscall_regs * regs)
{
#ifdef IN_INJECTOR
	int32_t retval = regs->eax;
	if (retval < 0) {
		/* failed */
		write_eax(regs);
		return 0;
	}

	/* check whether we support this clone */

	unsigned long clone_flags;
	unsigned long newsp;
	uintptr_t parent_tidptr, child_tidptr;

	clone_flags = regs->ebx;
	newsp = regs->ecx;
	parent_tidptr = regs->edx;
	child_tidptr = regs->edi;
	if (!newsp)
		newsp = regs->esp;

	if (retval == 0) {
		/* child */
		do_child_fork(clone_flags, newsp, regs,
				parent_tidptr, child_tidptr);
	} else {
		write_eax(regs);
		/* write the flags */
		write_obj(clone_flags);
		if (clone_flags & CLONE_PARENT_SETTID) {
			ASSERT(parent_tidptr != 0, "!@!@#$%^@#$\n");
			write_mem(parent_tidptr, sizeof(long));
		}

		do_parent_fork(clone_flags, newsp, regs,
				parent_tidptr, child_tidptr);
	}
#else
	INJ_ERROR("call clone in load phase\n");
	exit(-1);
#endif
	return 0;
}

int SCOPE
replay_clone(const struct syscall_regs * regs)
{
	int32_t eax = read_eax();
	if (eax > 0) {
		uint32_t clone_flags;
		read_obj(clone_flags);
		ASSERT(clone_flags == regs->ebx, "clone flags inconsistent\n");
		if (clone_flags & CLONE_PARENT_SETTID) {
			uintptr_t parent_tidptr = regs->edx;
			read_mem(parent_tidptr, sizeof(long));
		}
	}
	return eax;
}
#else

void
output_clone(void)
{
	int32_t eax = read_eax();
	printf("clone:\t%d\n", eax);
	if (eax > 0) {
		uint32_t clone_flags;
		read_obj(clone_flags);
		if (clone_flags & CLONE_PARENT_SETTID) {
			skip(sizeof(long));
		}
	}
}
#endif

