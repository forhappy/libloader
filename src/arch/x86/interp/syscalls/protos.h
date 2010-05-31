/* 
 * protos.h
 * by WN @ May. 27, 2010
 */

#ifndef PROTOS_H
#define PROTOS_H

#ifndef ATTR_HIDDEN
# define ATTR_HIDDEN __attribute__((visibility ("hidden")))
#endif

#include <xasm/unistd_32.h>
#include "syscall_table.h"

/* provide prototypes of system call handlers */

extern int
trival_pre_handler(struct pusha_regs * regs);

extern int
trival_post_handler(struct pusha_regs * regs);

extern int
trival_replay_handler(struct pusha_regs * regs);

#define __def_handler(name, pre, post, replay)	\
	extern int pre(struct pusha_regs * regs) ATTR_HIDDEN;	\
	extern int post(struct pusha_regs * regs) ATTR_HIDDEN;	\
	extern int replay(struct pusha_regs * regs) ATTR_HIDDEN;

#define def_trival_handler(name)

#define def_handler(name)	\
	extern int post_##name(struct pusha_regs * regs) ATTR_HIDDEN; \
	extern int replay_##name(struct pusha_regs * regs) ATTR_HIDDEN;

#define def_complex_handler(name)	\
	__def_handler(name, pre_##name, post_##name, replay_##name)

#include "handlers.h"

#undef def_complex_handler
#undef def_handler
#undef def_trival_handler
#undef __def_handler

#endif

