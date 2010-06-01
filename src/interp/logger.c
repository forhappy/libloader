/* 
 * logger.c
 * by WN @ Mar. 17, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <interp/code_cache.h>
#include <interp/mm.h>
#include <interp/logger.h>
#include <interp/checkpoint.h>
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
#include <xasm/types_helper.h>

#include <zlib/zlib.h>

/* 
 * the format of log file:
 *
 * mark(1byte)+data
 *
 * mark: 0x1: compressed data
 * mark: 0x0: uncompressed data
 *
 * if mark == 0x1:
 *   data ::= comp-head compressed-data
 *   comp-head ::= real_sz compressed_sz
 * else if mark == 0x0:
 *   data ::= uncomp-head uncompressed-data
 *   uncomp-head ::= real_sz
 */

static void
reset_fns(struct tls_logger * logger, int pid, int tid)
{
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
			LOG_DIR, pid, tid,
			(uint32_t)tv.tv_sec, (uint32_t)tv.tv_usec);
	assert(fn_len < MAX_LOGGER_FN);

	fn_len = snprintf(logger->ckpt_fn, MAX_CKPT_FN,
			"%s/%d-%d-%010u-%010u.ckpt",
			LOG_DIR, pid, tid,
			(uint32_t)tv.tv_sec, (uint32_t)tv.tv_usec);
	assert(fn_len < MAX_CKPT_FN);
	TRACE(LOGGER, "logger file name: %s\n", logger->log_fn);
	TRACE(LOGGER, "checkpoint file name: %s\n", logger->ckpt_fn);
}

void
init_logger(struct tls_logger * logger, int pid, int tid)
{
	logger->check_logger_buffer = check_logger_buffer;

	logger->log_buffer_start = alloc_pages(LOG_PAGES_NR, FALSE);
	assert(logger->log_buffer_start != NULL);

	logger->log_buffer_current = logger->log_buffer_start;
	/* additional bytes! see branch_template.S */
	logger->log_buffer_end = logger->log_buffer_start + MAX_LOGGER_SIZE;

	prepare_tls_compress(&logger->compress,
			LOG_PAGES_NR * PAGE_SIZE);

	/* generate log and checkpoint file name */
	reset_fns(logger, pid, tid);
}

void
close_logger(struct tls_logger * logger)
{
	if (logger->log_buffer_start == NULL)
		return;
	free_pages(logger->log_buffer_start, LOG_PAGES_NR);
	destroy_tls_compress(&logger->compress);
	/* don't use memset: the file name of logger and ckpt
	 * reside in the logger structure */
	logger->check_buffer_return = NULL;
	logger->check_logger_buffer = NULL;
	logger->log_buffer_start = NULL;
	logger->log_buffer_current = NULL;
	logger->log_buffer_end = NULL;
}

static void *
compress_logger_buffer(uint8_t * start, int in_sz, unsigned int * p_out_sz,
		struct tls_compress * pcomp)
{
	/* compress log data and print compressed size */

	uint8_t * out_buf = NULL;
	unsigned int out_sz = 0;
	compress(pcomp, start, in_sz, &out_buf, &out_sz);
	assert(out_buf != NULL);
	assert(out_sz != 0);
	*p_out_sz = out_sz;
	DEBUG(LOGGER, "flush logger buffer: ori sz=%d, compressed sz=%d\n",
			in_sz, out_sz);
	return out_buf;
}

static int
open_logger_file(struct tls_logger * logger)
{
	int fd = INTERNAL_SYSCALL_int80(open, 3, logger->log_fn,
			O_WRONLY|O_APPEND|O_CREAT, 0664);
	if (fd <= 0)
		FATAL(LOGGER, "open logger file %s failed: %d\n",
				logger->log_fn, fd);
	TRACE(LOGGER, "log file %s opened, fd=%d\n", logger->log_fn, fd);
	return fd;
}

static void
write_to_logger_file(int fd, void * data, size_t size)
{
	int err = INTERNAL_SYSCALL_int80(write, 3, fd, data,
			size);
	assert(err == (int)(size));
}

static void
close_logger_file(int fd)
{
	int err;
	err = INTERNAL_SYSCALL_int80(close, 1, fd);
	TRACE(LOGGER, "close %d: %d\n", fd, err);
	assert(err == 0);
#warning "also need to trigger ckpt switching here"
}

static void
do_flush_logger_buffer(struct tls_logger * logger)
{
	DEBUG(LOGGER, "----------- flush logger buffer ------------\n");
	void * compressed_data;
	unsigned int out_sz;
	unsigned int log_sz = logger->log_buffer_current -
		logger->log_buffer_start;

	compressed_data = compress_logger_buffer(logger->log_buffer_start,
			logger->log_buffer_current - logger->log_buffer_start,
			&out_sz, &logger->compress);
	assert(compressed_data != NULL);
	assert(out_sz != 0);

	int fd = open_logger_file(logger);

	TRACE(LOGGER, "log file %s opened, fd=%d\n", logger->log_fn, fd);

	/* write mark */

	uint32_t mark = COMPRESSED_DATA_MARK;
	write_to_logger_file(fd, &mark, sizeof(mark));

	struct log_block_tag tag = {
		.real_sz = log_sz,
		.compressed_sz = out_sz,
	};

	/* write the tag */
	write_to_logger_file(fd, &tag, sizeof(tag));
	write_to_logger_file(fd, compressed_data, out_sz);

	close_logger_file(fd);
#warning "needs to trigger ckpt switching here"
}

/* A straightforward idea is to fork a new process and put
 * the flushing and checkpointing works there. for checkpoint,
 * it is okay. However, for log flushing, it has problems.
 * 
 * Think about when a process flush twice and we fork 2 new process
 * to do the flushing. Because of the scheduling problem, the 2nd
 * process may start before the first one. Without any synchronization,
 * the newer log will be written first, disturbs the log.
 *
 * Checkpoint and checkpoint switching take more problems for forking
 * flushing.
 *
 * Therefore, although forking flushing is attractive,
 * we decide not to adopt it. 
 *
 * However, I know many methods can solve this problem. One is ticket
 * mechanism. The principle is: 1. target process puts a mark into
 * file system; 2. child process does the flushing work and removes the
 * mark before exit.
 *
 *  1. the parent process check whether to do checkpointing, if not:
 *  2. check file system, find the newest mark of current checkpoint;
 *  3. put a newer mark into file system, and fork the child process
 *  4. child process wait the previous mark disappear, do the flushing
 *     work and remove the newer mark.
 * */
static void
flush_logger_buffer(struct tls_logger * logger)
{
	do_flush_logger_buffer(logger);

	/* clear logger status and return */
	int sz = (uintptr_t)(logger->log_buffer_end) -
		(uintptr_t)(logger->log_buffer_start) + LOGGER_ADDITIONAL_BYTES;
	memset(logger->log_buffer_start, '\0', sz);
	logger->log_buffer_current = logger->log_buffer_start;
	return;

}

void
do_check_logger_buffer(void)
{
	struct thread_private_data * tpd = get_tpd();
	if (tpd->logger.log_buffer_current < tpd->logger.log_buffer_end)
		return;
	BLOCK_SIGNALS(tpd);
	if (tpd->logger.log_buffer_current >= tpd->logger.log_buffer_end) {
		/* we need check buffer again because signal may raise after
		 * the previous check and before we block signal */
		flush_logger_buffer(&tpd->logger);
	}
	UNBLOCK_SIGNALS(tpd);
	return;
}

void
flush_logger(void)
{
	struct thread_private_data * tpd = get_tpd();
	do_flush_logger_buffer(&tpd->logger);
}

void
append_buffer(void * data, size_t size)
{

	assert(data != NULL);
	assert(size != 0);

	struct thread_private_data * tpd = get_tpd();
	struct tls_logger * logger = &tpd->logger;

	TRACE(LOGGER, "append %d bytes into buffer\n", size);

	if (logger->log_buffer_current + size <
			logger->log_buffer_end)
	{
		/* we have enough space */
		memcpy(logger->log_buffer_current, data, size);
		logger->log_buffer_current += size;
		return;
	}

	/* we don't have enough space */
	if (logger->log_buffer_current > logger->log_buffer_start) {
		BLOCK_SIGNALS(tpd);
		flush_logger_buffer(logger);
		UNBLOCK_SIGNALS(tpd);
	}

	if (logger->log_buffer_current + size <
			logger->log_buffer_end)
	{
		/* now we have enough space */
		memcpy(logger->log_buffer_current, data, size);
		logger->log_buffer_current += size;
		return;
	}

	assert(size > MAX_LOGGER_SIZE);
	/* direct write */
	int fd = open_logger_file(logger);
	uint32_t mark = UNCOMPRESSED_DATA_MARK;
	write_to_logger_file(fd, &mark, sizeof(mark));
	write_to_logger_file(fd, data, size);
	close_logger_file(fd);

	return;
}

/* ********** replay code ********** */
void
load_from_buffer(void * data ATTR_UNUSED, size_t size ATTR_UNUSED)
{
#warning not implemented!!!
}

// vim:ts=4:sw=4

