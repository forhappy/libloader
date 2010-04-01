/* 
 * compress.c
 * by WN @ Mar. 31, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <interp/mm.h>
#include <interp/compress.h>
#include <xasm/tls.h>
#include <xasm/string.h>

#ifdef USE_ZLIB

#include <zlib/zlib.h>

static void
__zlib_prepare_tls_base(struct tls_compress * pcomp, int buffer_sz,
		int workspace_sz)
{
	/* according to the docs of zlib, the helper buffer
	 * should be at least 100.01 * (buffer_sz + 12bytes) */
	TRACE(COMPRESS, "prepare zlib tls base: buffer_sz=0x%x, workspace_sz=0x%x\n",
			buffer_sz, workspace_sz);
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

#define get_tls_compress() ({struct thread_private_data * tpd = get_tpd();\
		&(tpd->compress);})

static void
__zlib_prepare_tls_inflate(int buffer_sz ATTR(unused))
{
	TRACE(COMPRESS, "prepare zlib inflate tls\n");
	TRACE(COMPRESS, "nothing to do\n");
}

static void
__zlib_prepare_tls_deflate(int buffer_sz)
{
	struct tls_compress * pcomp = get_tls_compress();
	__zlib_prepare_tls_base(pcomp, buffer_sz,
			zlib_deflate_workspacesize());
}

static void
__zlib_destroy_tls(void)
{
	struct tls_compress * pcomp = get_tls_compress();
	
	if (pcomp->work_buffer1 != NULL)
		free_cont_space2(pcomp->work_buffer1, pcomp->sz1);
	pcomp->work_buffer1 = NULL;
	pcomp->sz1 = 0;
	pcomp->uncompress_buffer_sz = 0;

	if (pcomp->work_buffer2 != NULL)
		free_cont_space(pcomp->work_buffer2);
	pcomp->work_buffer2 = NULL;
	pcomp->sz2 = 0;
}


#define DEFLATE_DEF_LEVEL		Z_DEFAULT_COMPRESSION
#define DEFLATE_DEF_WINBITS		11
#define DEFLATE_DEF_MEMLEVEL		MAX_MEM_LEVEL

static void
__zlib_compress(const uint8_t * in_buf, int in_sz,
		const uint8_t ** pout_buf, int * out_sz)
{
	struct tls_compress * pcomp = get_tls_compress();
	assert(pcomp->work_buffer1);
	assert(pcomp->work_buffer2);
	assert(pcomp->sz2 == zlib_deflate_workspacesize());

	TRACE(COMPRESS, "zlib compress: in_sz=0x%x\n", in_sz);

	struct z_stream_s stream;
	stream.workspace = pcomp->work_buffer2;
	memset(stream.workspace, '\0', pcomp->sz2);

	/* -DEFLATE_DEF_WINBITS is undocumented feature: suppress zlib header */
	int err = zlib_deflateInit2(&stream, DEFLATE_DEF_LEVEL,
			Z_DEFLATED, -DEFLATE_DEF_WINBITS, DEFLATE_DEF_MEMLEVEL,
			Z_DEFAULT_STRATEGY);
	assert(err == Z_OK);

	err = zlib_deflateReset(&stream);
	assert(err == Z_OK);
	stream.next_in = (uint8_t*)(in_buf);
	stream.avail_in = in_sz;
	stream.next_out = (uint8_t*)(pcomp->work_buffer1);
	stream.avail_out = pcomp->sz2;

	err = zlib_deflate(&stream, Z_FINISH);
	assert(err == Z_STREAM_END);
	*pout_buf = pcomp->work_buffer1;
	*out_sz = stream.total_out;

	zlib_deflateEnd(&stream);

	TRACE(COMPRESS, "zlib compress complete, output buffer size=%d\n", *out_sz);
}

static void
__zlib_decompress(const uint8_t * in_buf, int in_sz,
		uint8_t * out_buf, int * out_sz)
{
	/* alloc workspace */
	assert(out_sz != NULL);
	assert(*out_sz != 0);

	int workspace_sz = zlib_inflate_workspacesize();
	void * workspace = alloc_cont_space(workspace_sz);
	assert(workspace != NULL);
	memset(workspace, '\0', workspace_sz);

	struct z_stream_s stream;
	stream.workspace = workspace;

	int err = zlib_inflateInit2(&stream, -DEFLATE_DEF_WINBITS);
	assert(err == Z_OK);
	err = zlib_inflateReset(&stream);
	assert(err == Z_OK);

	stream.next_in = (uint8_t*)(in_buf);
	stream.avail_in = in_sz;
	stream.next_out = (uint8_t*)(out_buf);
	stream.avail_out = *out_sz;

	err = zlib_inflate(&stream, Z_SYNC_FLUSH);

	assert(err == Z_STREAM_END);
	*out_sz = stream.total_out;
	free_cont_space(workspace);
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
compress(const uint8_t * in_buf, int in_sz,
	        const uint8_t ** pout_buf, int * out_sz)
{
	assert(in_buf != NULL);
	assert(in_sz >= 0);
	assert(pout_buf != NULL);
	assert(out_sz != NULL);
#ifdef USE_ZLIB
	__zlib_compress(in_buf, in_sz, pout_buf, out_sz);
#elif USE_LZO
	__lzo_compress(in_buf, in_sz, pout_buf, out_sz);
#else
# error no compression algorithm is selected
#endif
}

void
decompress(const uint8_t * in_buf, int in_sz,
		uint8_t * out_buf, int * out_sz)
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

