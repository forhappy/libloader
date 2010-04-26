/* 
 * user_entry.c
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <xasm/processor.h>
#include <xasm/tls.h>
#include <interp/user_entry.h>
#include <interp/checkpoint.h>

void
enter_user_code(struct pusha_regs * regs, void * eip)
{
	TRACE(LOADER, "generate the first checkpoint\n");
	fork_make_checkpoint(regs, (eip));
}

// vim:ts=4:sw=4

