/* 
 * syscall-handlers.h
 * by WN @ Jun. 25, 2009
 */

#ifndef __SYSCALL_HANDLERS_H
#define __SYSCALL_HANDLERS_H

#include <asm/unistd.h>
#include "checkpoint.h"
#include <stdint.h>

__BEGIN_DECLS

#define define_pre_handler(name)	\
	extern SCOPE int pre_##name(struct syscall_regs * regs)

#define define_post_handler(name)	\
	extern SCOPE int post_##name(struct syscall_regs * regs)

extern SCOPE int logger_fd;

#define write_syscall_nr(nr)	do {	\
	uint32_t x_nr;	\
	x_nr = nr;	\
	__write(logger_fd, &x_nr, sizeof(x_nr));	\
} while(0)

#define write_regs(regs)	do {	\
	__write(logger_fd, regs, sizeof(*regs));	\
} while(0)

#define write_eax(regs)	do {	\
	__write(logger_fd, &regs->eax, sizeof(regs->eax));	\
} while(0)

#define write_mem(d, sz) do {	\
	__write(logger_fd, d, sz);	\
} while(0)

define_post_handler(brk);
define_post_handler(uname);
define_post_handler(mmap2);
define_post_handler(access);

__END_DECLS

#endif

