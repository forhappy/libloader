

#include "syscalls.h"

#include <fcntl.h>
#include <unistd.h>

#ifndef SYSCALL_PRINTER

int SCOPE
post_open(const struct syscall_regs * regs)
{
	write_eax(regs);
	return 0;
}

//static char dev_zero_str[] = "/dev/zero";

int SCOPE
replay_open(const struct syscall_regs * regs)
{
	/* we need open files for mmap... */
	int32_t eax = read_int32();
#ifdef IN_INJECTOR
	if (eax >= 0) {
		int32_t ret;
		/* unset O_EXCL */
		uint32_t flags = regs->ecx & (~(O_EXCL));

		ret = INTERNAL_SYSCALL(open, 3, regs->ebx, flags, regs->edx);
		if (ret < 0) {
			INJ_WARNING("open file %s error: retval=%d, should be %d, open /dev/zero instead\n",
				regs->ebx, ret, eax);

			uint32_t new_flags = flags & (~(O_CREAT));
			ret = INTERNAL_SYSCALL(open, 3, "/dev/zero", new_flags, regs->edx);
			ASSERT(ret >= 0, regs, "open /dev/zero with f=0x%x, m=0x%x failed: %d\n",
					new_flags, regs->edx, ret);
		}

		/* if still < 0, error... */
		ASSERT(ret >= 0, regs, "open file %s with flags=0x%x, mode=0x%x still error: %d\n",
				regs->ebx, flags, regs->edx, ret);

		/* dup fd and close the original one */
		if (ret != eax) {
			int err;
			err = INTERNAL_SYSCALL(dup2, 2, ret, eax);
			ASSERT(err >= 0, regs, "dup2 from %d to %d failed\n",
					ret, eax);
			/* close old file */
			err = INTERNAL_SYSCALL(close, 1, ret);
			ASSERT(err >= 0, regs, "close file %d failed\n", ret);
		}
	}
#endif
	return eax;
}



#else

void
output_open(void)
{
	printf("open:\t%d\n", read_eax());
}
#endif

