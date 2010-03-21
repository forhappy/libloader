/* 
 * logger.c
 * by WN @ Mar. 17, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <interp/code_cache.h>
#include <interp/mm.h>
#include <interp/dict.h>
#include <asm/utils.h>
#include <asm/tls.h>
#include <asm/logger.h>
#include <asm/compiler.h>

void
init_logger(void)
{
	struct thread_private_data * tpd = get_tpd();
	tpd->logger.check_logger_buffer = check_logger_buffer;

	tpd->logger.log_buffer_start = alloc_pages(LOG_PAGES_NR, FALSE);
	assert(tpd->logger.log_buffer_start != NULL);

	tpd->logger.log_buffer_current = tpd->logger.log_buffer_start;
	/* for additional bytes! see branch_template.S */
	tpd->logger.log_buffer_end = tpd->logger.log_buffer_start +
		LOG_PAGES_NR * PAGE_SIZE - 4;

}

void
heavy_log_branch_target(void)
{
	struct thread_private_data * tpd = get_tpd();
	VERBOSE(LOGGER, "target is %p\n", tpd->target);
}

// vim:ts=4:sw=4

