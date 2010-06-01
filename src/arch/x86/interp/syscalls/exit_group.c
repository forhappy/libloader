/* 
 * exit_group.c
 * by WN @ Jun. 01, 2010
 */

#include "syscall_handler.h"

#include <common/debug.h>
#include <interp/logger.h>

#ifdef PRE_LIBRARY
int
pre_exit_group(struct pusha_regs * regs)
{
	/* we need to flush log */
	VERBOSE(SYSTEM, "program call exit_group(%d), flush log\n", regs->ebx);
	flush_logger();
	return 0;
}
#endif

#ifdef POST_LIBRARY
int
post_exit_group(struct pusha_regs * regs)
{
	/* never come here */
	return 0;
}
#endif

#ifdef REPLAY_LIBRARY
int
replay_exit_group(struct pusha_regs * regs)
{
	VERBOSE(SYSTEM, "program call exit_group(%d) and exit\n", regs->ebx);
	__exit(0);
	return 0;
}
#endif

// vim:ts=4:sw=4

