/* 
 * by WN @ Jun. 25, 2009
 */

#include <asm/unistd.h>
#include "syscall_table.h"
#include "syscalls.h"

SCOPE int
dummy(struct syscall_regs * regs)
{
	return 0;
}

SCOPE struct syscall_tabent syscall_table[] = {
	[__NR_brk]	= {NULL, &post_brk},
	[__NR_uname]	= {NULL, &post_uname},
	[__NR_mmap2]	= {NULL, &post_mmap2},
	[__NR_access]	= {NULL, &post_access},
	[__NR_open]	= {NULL, &post_open},
	[__NR_fstat64]	= {NULL, &post_fstat64},
	[__NR_close]	= {NULL, &post_close},
	[__NR_stat64]	= {NULL, &post_stat64},
	[__NR_read]	= {NULL, &post_read},
	[__NR_set_thread_area]	= {NULL, &post_set_thread_area},
	[__NR_mprotect]	= {NULL, &post_mprotect},
	[__NR_munmap]	= {NULL, &post_munmap},
	[__NR_set_tid_address]	= {NULL, &post_set_tid_address},
	[__NR_set_robust_list]	= {NULL, &post_set_robust_list},
	[__NR_rt_sigaction]	= {NULL, &post_rt_sigaction},
	[__NR_rt_sigprocmask]	= {NULL, &post_rt_sigprocmask},
	[__NR_ugetrlimit]	= {NULL, &post_ugetrlimit},
	[__NR_write]		= {NULL, &post_write},
	[__NR_nanosleep]	= {NULL, &post_nanosleep},
	[__NR_ioctl]		= {&pre_ioctl, &post_ioctl},
	[NR_SYSCALLS]	= {NULL, NULL},
};



