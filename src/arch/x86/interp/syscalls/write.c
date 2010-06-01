/* 
 * write.c
 * by WN @ Jun. 01, 2010
 */

#include "syscall_handler.h"
#include <common/debug.h>
#include <xasm/syscall.h>

#ifdef PRE_LIBRARY
/* do nothing */
#endif

#ifdef POST_LIBRARY
DEF_HANDLER(write)
{
	TRACE(LOG_SYSCALL, "write\n");
	EAX_AS_INT;
	return 0;
}
#endif

#ifdef REPLAY_LIBRARY
DEF_HANDLER(write)
{
	TRACE(LOG_SYSCALL, "write\n");
	int r = EAX_AS_INT;
	if (r > 0) {
		if ((regs->ebx == 1) || (regs->ebx == 2))
			INTERNAL_SYSCALL_int80(write, 3,
					regs->ebx, regs->ecx, regs->edx);
	}
	return 0;
}
#endif

// vim:ts=4:sw=4

