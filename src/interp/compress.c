/* 
 * compress.c
 * by WN @ Mar. 31, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <interp/mm.h>
#include <interp/compress.h>
#include <xasm/tls.h>

#ifdef USE_ZLIB

#include <zlib/zlib.h>

static void
__zlib_prepare_tls_base(struct tls_compress * pcomp, int buffer_sz,
		int workspace_sz)
{
	/* according to the docs of zlib, the helper buffer
	 * should be at least 100.01 * (buffer_sz + 12bytes) */
	int helper_sz = (buffer_sz + 12) +
					(buffer_sz + 12) / 100 + 1;
	
	assert(pcomp->work_buffer1 == NULL);
	pcomp->uncompress_buffer_sz = buffer_sz;
	pcomp->sz1 = helper_sz;
	pcomp->work_buffer1 = alloc_cont_space2(helper_sz);
	assert(pcomp->work_buffer1 != NULL);

	/* alloc its workspace */
	assert(pcomp->work_buffer2 == NULL);
	pcomp->sz2 = workspace_sz;
	pcomp->work_buffer2 = alloc_cont_space(pcomp->sz2);
	assert(pcomp->work_buffer2 != NULL);
}

static void
__zlib_prepare_tls_inflate(int buffer_sz)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_compress * pcomp = &tpd->compress;
	__zlib_prepare_tls_base(pcomp, buffer_sz,
			zlib_inflate_workspacesize());
}

static void
__zlib_prepare_tls_deflate(int buffer_sz)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_compress * pcomp = &tpd->compress;
	__zlib_prepare_tls_base(pcomp, buffer_sz,
			zlib_deflate_workspacesize());
}

static void
__zlib_destroy_tls(void)
{
	struct thread_private_data * tpd = get_tpd();
	struct tls_compress * pcomp = &(tpd->compress);
	
	assert(pcomp->work_buffer1 != NULL);
	free_cont_space2(pcomp->work_buffer1, pcomp->sz1);
	pcomp->work_buffer1 = NULL;
	pcomp->sz1 = 0;
	pcomp->uncompress_buffer_sz = 0;

	free_cont_space(pcomp->work_buffer2);
	pcomp->sz2 = 0;
}

static void
__zlib_compress(uint8_t * in_buf, int in_sz,
		const uint8_t ** out_buf, int * out_sz)
{
	
}

static void
__zlib_decompress(uint8_t * in_buf, int in_sz,
		const uint8_t ** out_buf, int * out_sz)
{
	
}


#endif

#ifdef USE_LZO
# error unimplemented
#endif


void
prepare_tls_compress(int buffer_sz)
{
	assert(buffer_sz > 0);
#ifdef USE_ZLIB
	__zlib_prepare_tls_deflate(buffer_sz);
#elif USE_LZO
	__lzo_prepare_tls_compress(buffer_sz);
#else
# error no compression algorithm is selected
#endif
}

void
destroy_tls_compress(void)
{
#ifdef USE_ZLIB
	__zlib_destroy_tls();
#elif USE_LZO
	__lzo_destroy_tls_compress();
#else
# error no compression algorithm is selected
#endif
	
}

void
prepare_tls_decompress(int buffer_sz)
{
	assert(buffer_sz > 0);
#ifdef USE_ZLIB
	__zlib_prepare_tls_inflate(buffer_sz);
#elif USE_LZO
	__lzo_prepare_tls_compress(buffer_sz);
#else
# error no compression algorithm is selected
#endif
}

void
destroy_tls_decompress(void)
{
#ifdef USE_ZLIB
	__zlib_destroy_tls();
#elif USE_LZO
	__lzo_destroy_tls_compress();
#else
# error no compression algorithm is selected
#endif
}

void
compress(uint8_t * in_buf, int in_sz,
	        const uint8_t ** out_buf, int * out_sz)
{
	assert(in_buf != NULL);
	assert(in_sz >= 0);
	assert(out_buf != NULL);
	assert(out_sz != NULL);
#ifdef USE_ZLIB
	__zlib_compress(in_buf, in_sz, out_buf, out_sz);
#elif USE_LZO
	__lzo_compress(in_buf, in_sz, out_buf, out_sz);
#else
# error no compression algorithm is selected
#endif
}

void
decompress(uint8_t * in_buf, int in_sz,
		const uint8_t ** out_buf, int * out_sz)
{
	assert(in_buf != NULL);
	assert(in_sz >= 0);
	assert(out_buf != NULL);
	assert(out_sz != NULL);
#ifdef USE_ZLIB
	__zlib_decompress(in_buf, in_sz, out_buf, out_sz);
#elif USE_LZO
	__lzo_decompress(in_buf, in_sz, out_buf, out_sz);
#else
# error no compression algorithm is selected
#endif
}

// vim:ts=4:sw=4

