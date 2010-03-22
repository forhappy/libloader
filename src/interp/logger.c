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
#include <xasm/utils.h>
#include <xasm/tls.h>
#include <xasm/logger.h>
#include <xasm/compiler.h>
#include <xasm/kutils.h>
#include <xasm/string.h>

void
init_logger(void)
{
	struct thread_private_data * tpd = get_tpd();
	tpd->logger.check_logger_buffer = check_logger_buffer;

	tpd->logger.log_buffer_start = alloc_pages(LOG_PAGES_NR, FALSE);
	assert(tpd->logger.log_buffer_start != NULL);

	tpd->logger.log_buffer_current = tpd->logger.log_buffer_start;
	/* 4 additional bytes! see branch_template.S */
	tpd->logger.log_buffer_end = tpd->logger.log_buffer_start +
		LOG_PAGES_NR * PAGE_SIZE - 4;

}

static void
flush_logger_buffer(struct tls_logger * logger)
{
	int sz = (uintptr_t)(logger->log_buffer_end) -
		(uintptr_t)(logger->log_buffer_start) + 4;
	DEBUG(COMPILER, "----------- flush logger buffer ------------\n");
	memset(logger->log_buffer_start, '\0', sz);
	logger->log_buffer_current = logger->log_buffer_start;
}

void
do_check_logger_buffer(void)
{
	struct thread_private_data * tpd = get_tpd();
	if (tpd->logger.log_buffer_current < tpd->logger.log_buffer_end)
		return;
	block_signals();
	flush_logger_buffer(&tpd->logger);
	restore_signals();
	return;
}

// vim:ts=4:sw=4

