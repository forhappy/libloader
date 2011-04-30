/* 
 * bigbuffer.b
 * by WN @ Nov. 08, 2010
 */

#include <interp/bigbuffer.h>
#include <interp/compression.h>
#include <common/debug.h>
#include <xasm/syscall.h>
#include <linux/kernel.h>

#include <interp/rebranch_tpd.h>

static struct bigbuffer *
get_self_bb(void)
{
	struct thread_private_data * tpd = get_self_tpd();
	assert(tpd != NULL);
	struct bigbuffer * bb = &tpd->bb;
	return bb;
}

void
init_bigbuffer(struct bigbuffer * bb)
{
	assert(bb != NULL);
	TRACE(MEM, "init bigbuffer %p\n", bb);
	reset_bigbuffer_ptr(bb);
}

void
init_self_bigbuffer(void)
{
	init_bigbuffer(get_self_bb());
}


void
flush_bigbuffer(int fd, struct bigbuffer * bb)
{
	assert(fd >= 0);
	if (bb->ptr != 0)
		flush_comp_buffer(fd, bb->buffer, bb->ptr);
	reset_bigbuffer_ptr(bb);
}

void
flush_self_bigbuffer(int fd)
{
	flush_bigbuffer(fd, get_self_bb());
}

void
append_bigbuffer(int fd, struct bigbuffer * bb, const void * buf, size_t sz)
{
	assert(bb != NULL);
	assert(bb->ptr < BIGBUFFER_SZ);
	assert(fd >= 0);

	if (sz == 0)
		return;

	do {
		size_t cp_sz = min((size_t)(BIGBUFFER_SZ - bb->ptr), sz);
		if (cp_sz > 0) {
			memcpy(bb->buffer + bb->ptr, buf, cp_sz);
			bb->ptr += cp_sz;
			buf += cp_sz;
			sz -= cp_sz;
		}

		if (sz > 0) {
			assert(bb->ptr == BIGBUFFER_SZ);
			flush_bigbuffer(fd, bb);
		}
	} while (sz > 0);
}

void
append_self_bigbuffer(int fd, const void * buf, size_t sz)
{
	append_bigbuffer(fd, get_self_bb(), buf, sz);
}

// vim:ts=4:sw=4

