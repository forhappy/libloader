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
#include <interp/compress.h>
#include <xasm/utils.h>
#include <xasm/tls.h>
#include <xasm/logger.h>
#include <xasm/compiler.h>
#include <xasm/kutils.h>
#include <xasm/string.h>

#include <zlib/zlib.h>

void
init_logger(void)
{
	struct thread_private_data * tpd = get_tpd();
	tpd->logger.check_logger_buffer = check_logger_buffer;

	tpd->logger.log_buffer_start = alloc_pages(LOG_PAGES_NR, FALSE);
	assert(tpd->logger.log_buffer_start != NULL);

	tpd->logger.log_buffer_current = tpd->logger.log_buffer_start;
	/* additional bytes! see branch_template.S */
	tpd->logger.log_buffer_end = tpd->logger.log_buffer_start +
		LOG_PAGES_NR * PAGE_SIZE - LOGGER_ADDITIONAL_BYTES;

	prepare_tls_compress(LOG_PAGES_NR * PAGE_SIZE);
}

void
close_logger(void)
{
	struct thread_private_data * tpd = get_tpd();
	free_pages(tpd->logger.log_buffer_start, LOG_PAGES_NR);
	destroy_tls_compress();
}

static void
do_flush_logger_buffer(uint8_t * start, int sz)
{
	/* compress log data and print compressed size */
	const uint8_t * out_buf = NULL;
	int out_sz = 0;
	compress(start, sz, &out_buf, &out_sz);
	assert(out_buf != NULL);
	assert(out_sz != 0);
	VERBOSE(LOGGER, "flush logger buffer: ori sz=%d, compress sz=%d\n",
			sz, out_sz);
}

static void
flush_logger_buffer(struct tls_logger * logger)
{
	/* we need check buffer again because signal may raise after
	 * the previous check and before we close signal */
	if (logger->log_buffer_current < logger->log_buffer_end)
		return;
	int sz = (uintptr_t)(logger->log_buffer_end) -
		(uintptr_t)(logger->log_buffer_start) + LOGGER_ADDITIONAL_BYTES;
	VERBOSE(COMPILER, "----------- flush logger buffer ------------\n");

	do_flush_logger_buffer(logger->log_buffer_start,
			logger->log_buffer_current -
			logger->log_buffer_start);

	memset(logger->log_buffer_start, '\0', sz);
	logger->log_buffer_current = logger->log_buffer_start;
}

void
do_check_logger_buffer(void)
{
	struct thread_private_data * tpd = get_tpd();
	if (tpd->logger.log_buffer_current < tpd->logger.log_buffer_end)
		return;
	BLOCK_SIGNALS(tpd);
	flush_logger_buffer(&tpd->logger);
	UNBLOCK_SIGNALS(tpd);
	return;
}

// vim:ts=4:sw=4

