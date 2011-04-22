/*
 * =====================================================================================
 *
 *       Filename:  syscall_tbl.h
 *
 *    Description: syscall table 
 *
 *        Version:  1.0
 *        Created:  04/11/2011 09:45:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: HP.Fu 
 *        Company: ICT
 *
 * =====================================================================================
 */
#ifndef __SYSCALL_TBL_H
#define __SYSCALL_TBL_H
#include <loader/snitchaser_main.h>
#include <syscall.h>
#include "syscall_nr.h"
__BEGIN_DECLS
struct syscall_regs {
	int32_t eax;
	int32_t ebx;
	int32_t ecx;
	int32_t edx;
	int32_t esi;
	int32_t edi;
	int32_t ebp;
	int32_t flags;
	int32_t orig_eax;
	int32_t eip;
	int32_t esp;
};
typedef int (*syscall_handler_t)(const const struct syscall_regs * r);

struct syscall_tabent {
	syscall_handler_t pre_handler;
	syscall_handler_t post_handler;
};

#define NR_SYSCALLS	(327)
extern SCOPE struct syscall_tabent syscall_table[NR_SYSCALLS + 1];


#define mkstr(x)	#x

# define def_syscall_entry(name, pre)	\
	[__NR_##name] = {pre, &post_##name}
# define def_simple_syscall_entry(name)	\
	[__NR_##name] = {NULL, &post_simple}



#define define_pre_handler(name)	\
	extern SCOPE int pre_##name(const struct syscall_regs * regs)

#define define_post_handler(name)	\
	extern SCOPE int post_##name(const struct syscall_regs * regs);	\

extern SCOPE int logger_fd;
define_post_handler(brk);
define_post_handler(mmap);
define_post_handler(mmap2);
define_post_handler(open);
define_post_handler(fstat64);
define_post_handler(close);
define_post_handler(stat64);
define_post_handler(read);
define_post_handler(mprotect);
define_post_handler(munmap);
define_post_handler(write);
define_post_handler(ioctl);
define_post_handler(gettimeofday);
define_post_handler(getpid);
define_post_handler(rt_sigprocmask);
define_post_handler(rt_sigaction);
define_post_handler(nanosleep);
define_post_handler(set_tid_address);
define_post_handler(set_robust_list);
define_post_handler(set_thread_area);
define_post_handler(futex);
define_post_handler(ugetrlimit);
define_post_handler(uname);
define_post_handler(statfs64);
__END_DECLS
#endif

