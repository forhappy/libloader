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

__END_DECLS

#endif

