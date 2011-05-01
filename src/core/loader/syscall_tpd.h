/* 
 * syscall_tpd.h
 * by WN @ Nov. 29, 2010
 */

#ifndef INTERP_SYSCALL_NR
#define INTERP_SYSCALL_NR

#include <loader/processor.h>

struct syscall_tpd_stub {
	int curr_syscall_nr;
	bool_t is_posted;
	struct pusha_regs bkup_regs;
	void * bkup_stack_top;
};

#endif

// vim:ts=4:sw=4

