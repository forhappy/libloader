/* 
 * syscall_table.h
 * by WN @ Jun. 25, 2009
 */

#ifndef __SYSCALL_TABLE_H
#define __SYSCALL_TABLE_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <unistd.h>
#include "defs.h"

__BEGIN_DECLS

#ifndef IN_INJECTOR
# define SCOPE
#else
# define SCOPE ATTR(visibility ("hidden"))
#endif

struct syscall_regs {
	int32_t orig_eax;
	int32_t eax;
	int32_t ebx;
	int32_t ecx;
	int32_t edx;
	int32_t esi;
	int32_t edi;
	int32_t ebp;
	int32_t eip;
};


typedef int (*syscall_handler_t)(struct syscall_regs * r);
struct syscall_tabent {
	syscall_handler_t pre_handler;
	syscall_handler_t post_handler;
};

extern SCOPE struct syscall_tabent syscall_table[];

#define NR_SYSCALLS	(327)

__END_DECLS

#endif

