/* 
 * log_syscalls.h
 * by WN @ May. 25, 2010
 */

#ifndef LOG_SYSCALLS_H
#define LOG_SYSCALLS_H

#include <xasm/processor.h>

extern void
pre_log_syscall_int80(struct pusha_regs regs);

extern void
post_log_syscall_int80(struct pusha_regs regs);

extern void
pre_log_syscall_vdso(struct pusha_regs regs);

extern void
post_log_syscall_vdso(struct pusha_regs regs);

#endif

// vim:ts=4:sw=4

