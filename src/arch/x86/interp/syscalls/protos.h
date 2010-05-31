/* 
 * protos.h
 * by WN @ May. 27, 2010
 */

#ifndef PROTOS_H
#define PROTOS_H

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
	extern int pre(struct pusha_regs * regs);	\
	extern int post(struct pusha_regs * regs);	\
	extern int replay(struct pusha_regs * regs);

#define def_trival_handler(name)

#define def_handler(name)	\
	__def_handler(name, trival_pre_handler, post_##name, replay_##name)

#define def_complex_handler(name)	\
	__def_handler(name, pre_##name, post_##name, replay_##name)

#include "handlers.h"

#undef def_complex_handler
#undef def_handler
#undef def_trival_handler
#undef __def_handler

#endif

