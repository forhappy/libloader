/* 
 * by WN @ Jun. 25, 2009
 */

#include <asm/unistd.h>
#include "checkpoint.h"
#include "syscalls.h"

#define mkstr(x)	#x

SCOPE int
dummy(const struct syscall_regs * regs)
{
	return 0;
}

#ifndef SYSCALL_PRINTER
# define def_syscall_entry(name, pre)	\
	[__NR_##name] = {pre, &post_##name, &replay_##name}
# define def_simple_syscall_entry(name)	\
	[__NR_##name] = {NULL, &post_simple, &replay_simple}
#else
# define def_syscall_entry(name, pre)	\
	[__NR_##name] = {&output_##name, mkstr(name)}
# define def_simple_syscall_entry(name)	\
	[__NR_##name] = {&output_simple, mkstr(name)}
#endif

SCOPE struct syscall_tabent syscall_table[] = {
	def_syscall_entry(brk, NULL),
	def_syscall_entry(uname, NULL),
	def_syscall_entry(mmap2, NULL),
	def_simple_syscall_entry(access),
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
	def_syscall_entry(rt_sigaction, pre_rt_sigaction),
	def_syscall_entry(rt_sigprocmask, NULL),
	def_syscall_entry(sigprocmask, NULL),
	def_syscall_entry(ugetrlimit, NULL),
	def_syscall_entry(write, NULL),
	def_syscall_entry(nanosleep, NULL),
	def_syscall_entry(ioctl, pre_ioctl),
	def_syscall_entry(fcntl64, NULL),
	def_syscall_entry(getdents64, NULL),
	def_syscall_entry(exit_group, pre_exit_group),
	def_syscall_entry(time, NULL),
	def_syscall_entry(socketcall, NULL),
	def_syscall_entry(_newselect, NULL),
	def_syscall_entry(_llseek, NULL),
	def_syscall_entry(clock_getres, NULL),
	def_syscall_entry(utime, NULL),
	def_syscall_entry(lstat64, NULL),
	def_syscall_entry(unlink, NULL),
	def_syscall_entry(futex, NULL),
	def_syscall_entry(clock_gettime, NULL),
	def_syscall_entry(getuid32, NULL),
	def_syscall_entry(getpid, NULL),
	def_syscall_entry(getcwd, NULL),
	def_syscall_entry(geteuid32, NULL),
	def_syscall_entry(getegid32, NULL),
	def_syscall_entry(getgid32, NULL),
	def_syscall_entry(poll, NULL),
	def_syscall_entry(writev, NULL),
	def_syscall_entry(sendfile64, NULL),
	def_syscall_entry(getrusage, NULL),
	def_syscall_entry(gettimeofday, NULL),
	def_syscall_entry(pipe, NULL),
	def_syscall_entry(clone, NULL),
	def_syscall_entry(kill, NULL),
	def_syscall_entry(getppid, NULL),
	def_simple_syscall_entry(alarm),
	def_syscall_entry(waitpid, NULL),
	def_syscall_entry(link, NULL),
	def_syscall_entry(dup, NULL),
	def_syscall_entry(dup2, NULL),
	def_syscall_entry(wait4, NULL),
	def_syscall_entry(setrlimit, NULL),
	def_syscall_entry(mknod, NULL),
	def_syscall_entry(mkdir, NULL),
	def_simple_syscall_entry(creat),
	def_simple_syscall_entry(rmdir),
#ifndef SYSCALL_PRINTER
	[NR_SYSCALLS]	= {NULL, NULL, NULL},
#else
	[NR_SYSCALLS]	= {NULL},
#endif
};

