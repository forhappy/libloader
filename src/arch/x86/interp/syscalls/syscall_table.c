/* 
 * syscall_table.c
 * by WN @ May. 27, 2010
 */

#include <common/defs.h>
#include <xasm/unistd_32.h>

#include "protos.h"
#include "syscall_table.h"


/* trival pre handler: do nothing */
/* trival post handler: save eax only */

#define __def_handler(name, pre, post, replay) [__NR_##name] = {pre, post, replay},

#define def_trival_handler(name)	__def_handler(name, trival_pre_handler,	\
	trival_post_handler, trival_replay_handler)

#define def_handler(name)	__def_handler(name, pre_##name, post_##name, replay_##name)

struct syscall_table_entry syscall_table[] = {
#include "handlers.h"
};

#undef def_handler
#undef def_trival_handler
#undef __def_handler

// vim:ts=4:sw=4

