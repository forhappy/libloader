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
	/* this is used for logger. call *%fs:0xxxx is
	 * easier to generate than call 0xxxxxxxxx.
	 * however, it may slower because call *%fs:0xxxx
	 * is an indirect jmp and require one more memory access */
	/* 4 fast logger entry: if log_space > 0, log the target into log_buffer_current;
	 * if not, call log_branch_target */
	/* ud_logger_entry should only be called when the first time
	 * the code compiled and run. after it called, the code cache
	 * should contain its target block, and the code is changed
	 * and jmp to target directly. */
	void * ud_logger_entry;
	/* if log buffer can hold target, it moves and transfers using assemble;
	 * if not, set fs:OFFSET_TARGET and call heavy_log_branch_target */
	void * log_branch_target;

	/* use 3 pointers to simplify asm code */
	void * log_buffer_start;
	void * log_buffer_current;
	void * log_buffer_end;
};


/* create logger's buffer and buffer size;
 * open logger file */
extern void
init_logger(void);

extern void
flush_logger_buffer(void);

/* target is got from tls storage fs:OFFSET_TARGET */
extern void
heavy_log_branch_target(void);

#endif

// vim:ts=4:sw=4

