/* 
 * logger_tpd.h
 * by WN @ Nov. 12, 2010
 */

#ifndef __LOGGER_TPD_H
#define __LOGGER_TPD_H

#include <config.h>
#include <common/defs.h>
#include <interp/bigbuffer.h>
#include <xasm/mutex.h>

struct logger_tpd_stub {
	char log_fn[MAX_OUTPUT_FN_LEN];
	struct bigbuffer * this_bigbuffer;
	void * start_ptr;
	/* real end ptr */
	void * end_ptr;

	struct xmutex log_flush_mutex;
	uintptr_t asm_curr_ptr;
	/* asm code use this sequence: write -> increase -> check
	 * (write ahead log), so logger must reserve some bytes to
	 * avoid asm code overwriting */
	uintptr_t asm_end_ptr;
};

#endif

// vim:ts=4:sw=4

