/* 
 * compression.c
 * by WN @ Nov. 07, 2010
 * modified by HP.Fu @ Apr 30, 2011
 */

#include <config.h>
#include <defs.h>
#include <linux/lzo.h>

#include <debug.h>

#include <loader/compression.h>

#include <syscall.h>
#include <alloca.h>

static void
flush_small_buffer(int fd, const void * buffer, size_t sz)
{
	assert(sz <= MAX_COMP_SZ);
	if (sz == 0)
		return;

	void * wrkmem = alloca(LZO1X_1_MEM_COMPRESS);
	assert(wrkmem != NULL);

	size_t destlen = lzo1x_worst_compress(sz);
	void * destbuffer = alloca(destlen);
	assert(destbuffer != NULL);

	int err = lzo1x_1_compress(buffer, sz, destbuffer, &destlen, wrkmem);
	assert(err == LZO_E_OK);

	/* flush */
	struct comp_block_head head = {
		.type = COMP_LZO_COMP,
		.ori_sz = sz,
		.comp_sz = destlen,
	};
	err = sys_write(fd, &head, sizeof(head));
	assert(err == sizeof(head));
	err = sys_write(fd, destbuffer, destlen);
	assert(err == destlen);
	return;
}

void
flush_comp_buffer(int fd, const void * buffer, size_t sz)
{
	assert(buffer != NULL);
	assert(fd >= 0);
	while (sz > 0) {
		size_t flushsz = sz > MAX_COMP_SZ ? MAX_COMP_SZ : sz;
		flush_small_buffer(fd, buffer, flushsz);
		buffer += flushsz;
		sz -= flushsz;
	}
}

int
decompress(const unsigned char *src, size_t src_len,
			unsigned char *dst, size_t *dst_len)
{
	/* wrapper of lzo1x_decompress_safe */
	assert(src != NULL);
	assert(dst != NULL);
	assert(dst_len != NULL);
	int err = lzo1x_decompress_safe(src, src_len, dst, dst_len);
	assert(err == LZO_E_OK);
	return 0;
}

void
write_comp_file_head(int fd)
{
	assert(fd >= 0);
	int err = sys_write(fd, COMP_FILE_HEADER, COMP_FILE_HEADER_SZ);
	assert(err == COMP_FILE_HEADER_SZ);
	return;
}

// vim:ts=4:sw=4

