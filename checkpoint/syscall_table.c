/* 
 * by WN @ Jun. 25, 2009
 */

#include <asm/unistd.h>
#include "checkpoint.h"
#include "syscalls.h"

SCOPE int
dummy(struct syscall_regs * regs)
{
	return 0;
}

#ifndef SYSCALL_PRINTER
# define def_syscall_entry(name, pre)	\
	[__NR_##name] = {pre, &post_##name}
#else
# define def_syscall_entry(name, pre)	\
	[__NR_##name] = {&output_##name}
#endif

SCOPE struct syscall_tabent syscall_table[] = {
	def_syscall_entry(brk, NULL),
	def_syscall_entry(uname, NULL),
	def_syscall_entry(mmap2, NULL),
	def_syscall_entry(access, NULL),
	def_syscall_entry(open, NULL),
	def_syscall_entry(fstat64, NULL),
	def_syscall_entry(close, NULL),
	def_syscall_entry(stat64, NULL),
	def_syscall_entry(read, NULL),
	def_syscall_entry(set_thread_area, NULL),
	def_syscall_entry(mprotect, NULL),
	def_syscall_entry(munmap, NULL),
	def_syscall_entry(set_tid_address, NULL),
	def_syscall_entry(set_robust_list, NULL),
	def_syscall_entry(rt_sigaction, NULL),
	def_syscall_entry(rt_sigprocmask, NULL),
	def_syscall_entry(sigprocmask, NULL),
	def_syscall_entry(ugetrlimit, NULL),
	def_syscall_entry(write, NULL),
	def_syscall_entry(nanosleep, NULL),
	def_syscall_entry(ioctl, &pre_ioctl),
	def_syscall_entry(fcntl64, NULL),
	def_syscall_entry(getdents64, NULL),
	def_syscall_entry(exit_group, pre_exit_group),
#ifndef SYSCALL_PRINTER
	[NR_SYSCALLS]	= {NULL, NULL},
#else
	[NR_SYSCALLS]	= {NULL},
#endif
};

