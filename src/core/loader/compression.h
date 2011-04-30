/* 
 * compression.h
 * by WN @ Nov. 07, 2010
 */

#ifndef __COMPRESSION_H
#define __COMPRESSION_H

#include <config.h>
#include <defs.h>

#define MAX_COMP_SZ		(BIGBUFFER_SZ)

#define COMP_FILE_HEADER	"SNITCHASER-LZO000"
#define COMP_FILE_HEADER_SZ	sizeof(COMP_FILE_HEADER)

enum comp_block_type {
	COMP_NOCOMP = 1,
	COMP_LZO_COMP = 2,
};

struct comp_block_head {
	enum comp_block_type type;
	size_t ori_sz;
	size_t comp_sz;
};

extern void
flush_comp_buffer(int fd, const void * buffer, size_t sz);

extern void
write_comp_file_head(int fd);

/* wrapper of lzo1x_decompress_safe */
extern int
decompress(const unsigned char *src, size_t src_len,
			unsigned char *dst, size_t *dst_len);

#endif

// vim:ts=4:sw=4

