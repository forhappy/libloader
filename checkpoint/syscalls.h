/* 
 * syscall-handlers.h
 * by WN @ Jun. 25, 2009
 */

#ifndef __SYSCALL_HANDLERS_H
#define __SYSCALL_HANDLERS_H

#include <asm/unistd.h>
#include "checkpoint.h"
#include <stdint.h>

#ifdef SYSCALL_PRINTER
# include <stdio.h>
# include <stdlib.h>
# include "exception.h"
# include "debug.h"
#endif

__BEGIN_DECLS

#ifndef SYSCALL_PRINTER
#define define_pre_handler(name)	\
	extern SCOPE int pre_##name(const struct syscall_regs * regs)

#define define_post_handler(name)	\
	extern SCOPE int post_##name(const struct syscall_regs * regs);	\
	extern SCOPE int replay_##name(const struct syscall_regs * regs)
#else
#define define_pre_handler(name)
#define define_post_handler(name)	\
	extern SCOPE void output_##name(void)
#endif

extern SCOPE int logger_fd;

define_post_handler(brk);
define_post_handler(uname);
define_post_handler(mmap2);
define_post_handler(access);
define_post_handler(open);
define_post_handler(fstat64);
define_post_handler(close);
define_post_handler(stat64);
define_post_handler(read);
define_post_handler(set_thread_area);
define_post_handler(mprotect);
define_post_handler(munmap);
define_post_handler(set_tid_address);
define_post_handler(set_robust_list);

define_pre_handler(rt_sigaction);
define_post_handler(rt_sigaction);

define_post_handler(rt_sigprocmask);
define_post_handler(sigprocmask);
define_post_handler(ugetrlimit);
define_post_handler(write);
define_post_handler(nanosleep);

define_pre_handler(ioctl);
define_post_handler(ioctl);

define_post_handler(fcntl64);
define_post_handler(getdents64);

define_pre_handler(exit_group);

define_post_handler(exit_group);
define_post_handler(time);
define_post_handler(socketcall);
define_post_handler(_newselect);
define_post_handler(_llseek);

define_post_handler(clock_getres);
define_post_handler(utime);
define_post_handler(lstat64);
define_post_handler(unlink);

define_post_handler(futex);

define_post_handler(clock_gettime);

define_post_handler(getuid32);
define_post_handler(getpid);
define_post_handler(getcwd);
define_post_handler(geteuid32);
define_post_handler(getegid32);
define_post_handler(getgid32);
define_post_handler(poll);

define_post_handler(writev);
define_post_handler(sendfile64);
define_post_handler(getrusage);
define_post_handler(gettimeofday);
define_post_handler(pipe);

define_post_handler(clone);

define_post_handler(kill);
define_post_handler(getppid);
define_post_handler(alarm);
define_post_handler(waitpid);
define_post_handler(link);

define_post_handler(dup);
define_post_handler(dup2);

__END_DECLS

#endif

