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
#include <xasm/vsprintf.h>
#include <xasm/utils.h>
#include <xasm/tls.h>
#include <xasm/logger.h>
#include <xasm/compiler.h>
#include <xasm/kutils.h>
#include <xasm/string.h>
#include <xasm/syscall.h>
#include <xasm/syscall_helper.h>

#include <zlib/zlib.h>

static void
reset_fns(struct thread_private_data * tpd)
{
	struct tls_logger * logger = &tpd->logger;
	struct timeval tv;
	int err = INTERNAL_SYSCALL_int80(gettimeofday, 2, &tv, NULL);
	assert(err == 0);
	/* compute the length of filenames */
	/* tv_sec and tv_usec are 32bits int, at most
	 * 10 decimal digit */
	/* filename: path/<pid>-<tid>-<timestamp> */
	/* path is LOG_DIR in config.h */
	int fn_len = snprintf(logger->log_fn, MAX_LOGGER_FN,
			"%s/%d-%d-%010u-%010u.log",
			LOG_DIR, tpd->pid, tpd->tid,
			(uint32_t)tv.tv_sec, (uint32_t)tv.tv_usec);
	assert(fn_len < MAX_LOGGER_FN);

	fn_len = snprintf(logger->ckpt_fn, MAX_CKPT_FN,
			"%s/%d-%d-%010u-%010u.ckpt",
			LOG_DIR, tpd->pid, tpd->tid,
			(uint32_t)tv.tv_sec, (uint32_t)tv.tv_usec);
	assert(fn_len < MAX_CKPT_FN);
	TRACE(LOGGER, "logger file name: %s\n", logger->log_fn);
	TRACE(LOGGER, "checkpoint file name: %s\n", logger->ckpt_fn);
}

void
init_logger(void)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_logger * logger = &tpd->logger;
	logger->check_logger_buffer = check_logger_buffer;

	logger->log_buffer_start = alloc_pages(LOG_PAGES_NR, FALSE);
	assert(logger->log_buffer_start != NULL);

	logger->log_buffer_current = logger->log_buffer_start;
	/* additional bytes! see branch_template.S */
	logger->log_buffer_end = logger->log_buffer_start +
		LOG_PAGES_NR * PAGE_SIZE - LOGGER_ADDITIONAL_BYTES;

	prepare_tls_compress(LOG_PAGES_NR * PAGE_SIZE);

	/* generate log and checkpoint file name */
	reset_fns(tpd);
}

void
close_logger(void)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_logger * logger = &tpd->logger;
	free_pages(logger->log_buffer_start, LOG_PAGES_NR);
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
	DEBUG(LOGGER, "flush logger buffer: ori sz=%d, compress sz=%d\n",
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
	DEBUG(COMPILER, "----------- flush logger buffer ------------\n");

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

