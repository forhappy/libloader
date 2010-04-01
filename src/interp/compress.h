/* 
 * compress.h
 * by WN @ Mar. 31, 2010
 */

#ifndef __COMPRESS_H
#define __COMPRESS_H

#include <common/defs.h>

struct tls_compress {
	int sz1;
	int sz2;
	int uncompress_buffer_sz;
	uint8_t * work_buffer1;
	uint8_t * work_buffer2;
};

void
prepare_tls_compress(int buffer_sz);

void
destroy_tls_compress(void);

void
prepare_tls_decompress(int buffer_sz);

void
destroy_tls_compress(void);

void
compress(const uint8_t * in_buf, int in_sz,
		const uint8_t ** pout_buf, int * out_sz);


/* decompress is different from compress:
 * it shouldn't occupy memory permanently. all workspace it
 * used during decompression should be alloced and freed dynamically. */
void
decompress(const uint8_t * in_buf, int in_sz,
		uint8_t * out_buf, int * out_sz);

#endif

// vim:ts=4:sw=4

