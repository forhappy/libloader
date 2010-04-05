/* 
 * logger.h
 * by WN @ Mar. 17, 2010
 */

#ifndef __LOGGER_H
#define __LOGGER_H

#include <interp/mm.h>

#define LOG_PAGES_NR	(1024*10)

/* 
 * TLS support for logger:
 * %fs:OFFSET_LOG_SPACE: the bytes buffer can hold
 * %fs:OFFSET_LOG_BUFFER: the buffer pointer
 * %fs:OFFSET_LOG_BUFFER_CURRENT: the buffer pointer
 */
#define MAX_LOGGER_FN	(128)
#define MAX_CKPT_FN		(MAX_LOGGER_FN)
struct tls_logger {
	/* check logger return addr */
	void * check_buffer_return;
	/* check logger nuffer */
	void * check_logger_buffer;

	/* use 3 pointers to simplify asm code */
	/* log buffer should contain at least 4 bytes
	 * additional space */
	void * log_buffer_start;
	void * log_buffer_current;
	void * log_buffer_end;

	/* the logger and checkpoint file */
	char log_fn[MAX_LOGGER_FN];
	char ckpt_fn[MAX_CKPT_FN];
};


/* create logger's buffer and buffer size;
 * open logger file */
extern void
init_logger(void);

/* close current log: when one thread exit, close its log */
extern void
close_logger(void);

#endif

// vim:ts=4:sw=4

