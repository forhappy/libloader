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
	[__NR_brk]	= {&dummy, &post_brk},
	[__NR_uname]	= {&dummy, &post_uname},
	[__NR_mmap2]	= {&dummy, &post_mmap2},
	[__NR_access]	= {&dummy, &post_access},
	[NR_SYSCALLS]	= {NULL, NULL},
};



