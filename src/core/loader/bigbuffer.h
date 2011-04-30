/* 
 * bigbuffer.h
 * by WN @ Nov. 08, 2010
 * modifed by HP.Fu @ Apr 30,2011
 *
 * Bigbuffer is embedded inside tpd.
 * Durning logging, it acts as logger buffer. Durning checking,
 * it is used to compress checkpoint.
 */

#ifndef __INTERP_BIGBUFFER_H
#define __INTERP_BIGBUFFER_H

#include <config.h>
#include <defs.h>
#include <asm/page.h>
#include <linux/lzo.h>

#define BIGBUFFER_PAGES		(((BIGBUFFER_SZ) + PAGE_SIZE - 1) / PAGE_SIZE)
#define BIGBUFFER_EXTRA_SZ	(lzo1x_worst_compress(BIGBUFFER_SZ) + LZO1X_1_MEM_COMPRESS)
#define BIGBUFFER_EXTRA_PAGES	(((BIGBUFFER_EXTRA_SZ) + (PAGE_SIZE - 1)) / PAGE_SIZE)

struct bigbuffer {
	uint8_t buffer[(BIGBUFFER_SZ)];
	/* ptr is the index of current byte, not C pointer. */
	uintptr_t ptr;
};

extern void
init_bigbuffer(struct bigbuffer * bb);

extern void
init_self_bigbuffer(void);

extern void
append_bigbuffer(int fd, struct bigbuffer * bb, const void * buf, size_t sz);

extern void
append_self_bigbuffer(int fd, const void * buf, size_t sz);

extern void
flush_bigbuffer(int fd, struct bigbuffer * bb);

extern void
flush_self_bigbuffer(int fd);

static inline void
reset_bigbuffer_ptr(struct bigbuffer * bb)
{
	bb->ptr = 0;
}

static inline void
set_bigbuffer_ptr(struct bigbuffer * bb, uintptr_t offset)
{
	bb->ptr = offset;
}

#endif

// vim:ts=4:sw=4

