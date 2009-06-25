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

#ifndef INJECTOR
# define SCOPE
#endif

struct syscall_regs {
	uint32_t orig_eax;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t eip;
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

