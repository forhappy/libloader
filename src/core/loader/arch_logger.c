/* 
 * arch_logger.c
 * by WN @ Nov. 18, 2010
 */

#include <common/debug.h>
#include <interp/rebranch_tpd.h>
#include <interp/logger.h>
#include <xasm/processor.h>

void
arch_flush_logger(struct pusha_regs * r)
{
	TRACE(LOGGER, "come to arch flush logger\n");
	struct logger_tpd_stub * l = get_self_logger_ptr();
	flush_logger(l);
}

// vim:ts=4:sw=4

