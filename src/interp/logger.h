/* 
 * logger.h
 * by WN @ Mar. 17, 2010
 */

#ifndef __LOGGER_H
#define __LOGGER_H

#define LOG_PAGES_NR	(1)

/* 
 * TLS support for logger:
 * %fs:OFFSET_LOG_SPACE: the bytes buffer can hold
 * %fs:OFFSET_LOG_BUFFER: the buffer pointer
 * %fs:OFFSET_LOG_BUFFER_CURRENT: the buffer pointer
 */

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
};


/* create logger's buffer and buffer size;
 * open logger file */
extern void
init_logger(void);

#endif

// vim:ts=4:sw=4

