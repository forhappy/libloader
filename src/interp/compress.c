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

# warning you choose to use zlib to compress log, it is not recommended.
# warning results show that LZO is both faster and smaller.

#include <zlib/zlib.h>

static void
__zlib_prepare_tls_base(struct tls_compress * pcomp, int buffer_sz,
		int workspace_sz)
{
	/* according to the docs of zlib, the helper buffer
	 * should be at least 100.01 * (buffer_sz + 12bytes) */
	TRACE(COMPRESS, "prepare zlib tls base: buffer_sz=0x%x, "
			"workspace_sz=0x%x\n",
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

static void
__zlib_prepare_tls_inflate(struct tls_compress * pcomp ATTR(unused),
		int buffer_sz ATTR(unused))
{
	TRACE(COMPRESS, "prepare zlib inflate tls\n");
	TRACE(COMPRESS, "nothing to do\n");
}

static void
__zlib_prepare_tls_deflate(struct tls_compress * pcomp,
		int buffer_sz)
{
	__zlib_prepare_tls_base(pcomp, buffer_sz,
			zlib_deflate_workspacesize());
}


static void
__zlib_destroy_tls(struct tls_compress * pcomp)
{
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
__do_zlib_compress(struct tls_compress * pcomp, const uint8_t * in_buf,
		int in_sz, const uint8_t * out_buf, unsigned int * out_sz)
{
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
	stream.next_out = (uint8_t*)(out_buf);
	stream.avail_out = *out_sz;

	err = zlib_deflate(&stream, Z_FINISH);

	if (err != Z_STREAM_END) {
		ERROR(COMPRESS, "error: stream.avail_in=%d, stream.avail_out=%d\n",
				stream.avail_in, stream.avail_out);
		FATAL(COMPRESS, "err=%d!=Z_STREAM_END\n", err);
	}

	*out_sz = stream.total_out;
	zlib_deflateEnd(&stream);
}

static void
__zlib_compress(struct tls_compress * pcomp,
		const uint8_t * in_buf, int in_sz,
		uint8_t ** pout_buf, unsigned int * p_out_sz)
{
	assert(pcomp->work_buffer1);
	assert(pcomp->work_buffer2);
	assert(pcomp->sz2 == zlib_deflate_workspacesize());
	assert(in_sz <= pcomp->sz1);

	TRACE(COMPRESS, "zlib compress: in_sz=0x%x\n", in_sz);

	unsigned int out_sz = pcomp->sz1;
	__do_zlib_compress(pcomp, in_buf, in_sz, pcomp->work_buffer1, &out_sz);

	*pout_buf = pcomp->work_buffer1;
	*p_out_sz = out_sz;

	TRACE(COMPRESS, "zlib compress complete, output buffer size=%d\n", out_sz);
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

#define __prepare_tls_deflate __zlib_prepare_tls_deflate
#define __prepare_tls_inflate __zlib_prepare_tls_inflate
#define __destroy_tls_compress	__zlib_destroy_tls
#define __destroy_tls_decompress	__zlib_destroy_tls
#define __compress	__zlib_compress
#define __compress2	__do_zlib_compress
#define __decompress __zlib_decompress

#endif

#ifdef USE_LZO

#include <lzo/lzo.h>

#define LZO1X_MEM_COMPRESS	(16384 * sizeof(unsigned char *))
#define LZO1X_1_MEM_COMPRESS	LZO1X_MEM_COMPRESS
#define lzo1x_worst_compress(x) ((x) + ((x) / 16) + 64 + 3)

static void
__lzo_prepare_tls_deflate(struct tls_compress * pcomp, int buffer_sz)
{
	TRACE(COMPRESS, "prepare lzo tls buffer_sz=0x%x\n", buffer_sz);
	int helper_sz = lzo1x_worst_compress(buffer_sz);

	/* alloc compress data space */
	assert(pcomp->work_buffer1 == NULL);
	pcomp->uncompress_buffer_sz = buffer_sz;
	pcomp->sz1 = helper_sz;
	pcomp->work_buffer1 = alloc_cont_space2(helper_sz);
	assert(pcomp->work_buffer1);

	/* alloc workspace */
	assert(pcomp->work_buffer2 == NULL);
	pcomp->sz2 = LZO1X_1_MEM_COMPRESS;
	/* different from zlib: we use alloc_cont_space2 because we know
	 * LZO1X_1_MEM_COMPRESS is aligned with page boundary. */
	pcomp->work_buffer2 = alloc_cont_space2(pcomp->sz2);
	assert(pcomp->work_buffer2 != NULL);
}


static void
__lzo_prepare_tls_inflate(struct tls_compress * pcomp ATTR(unused),
		int buffer_sz ATTR(unused))
{
	TRACE(COMPRESS, "prepare lzo tls inflate\n");
	TRACE(COMPRESS, "nothing to do\n");
}

static void
__lzo_destroy_tls(struct tls_compress * pcomp)
{
	if (pcomp->work_buffer1 != NULL)
		free_cont_space2(pcomp->work_buffer1, pcomp->sz1);
	pcomp->work_buffer1 = NULL;
	pcomp->sz1 = 0;
	pcomp->uncompress_buffer_sz = 0;

	/* different from zlib: use free_cont_space2 */
	if (pcomp->work_buffer2 != NULL)
		free_cont_space2(pcomp->work_buffer2, pcomp->sz2);
	pcomp->work_buffer2 = NULL;
	pcomp->sz2 = 0;
}

static void
__do_lzo_compress(struct tls_compress * pcomp, const uint8_t * in_buf,
		int in_sz, uint8_t * out_buf, unsigned int * p_out_sz)
{
	int err;
	err = lzo1x_1_compress(in_buf, in_sz, out_buf, p_out_sz,
			pcomp->work_buffer2);
	assert(err == LZO_E_OK);
}


static void
__lzo_compress(struct tls_compress * pcomp,
		const uint8_t * in_buf, int in_sz,
		uint8_t ** p_out_buf, unsigned int * p_out_sz)
{
	assert(pcomp->work_buffer1);
	assert(pcomp->work_buffer2);
	assert(in_sz <= pcomp->sz1);

	TRACE(COMPRESS, "lzo compress: in_sz=0x%x\n", in_sz);

	unsigned int out_sz = pcomp->sz1;
	__do_lzo_compress(pcomp, in_buf, in_sz, pcomp->work_buffer1, &out_sz);
	*p_out_sz = out_sz;
	*p_out_buf = pcomp->work_buffer1;
}

static void
__lzo_decompress(const uint8_t * in_buf, int in_sz,
		uint8_t * out_buf, int * p_out_sz)
{
	TRACE(COMPRESS, "lzo decompress\n");
	/* alloc workspace */
	void * workspace = alloc_cont_space2(LZO1X_1_MEM_COMPRESS);
	assert(workspace != NULL);

	int err;
	err = lzo1x_decompress_safe(in_buf, in_sz, out_buf, (unsigned int*)p_out_sz);
	assert(err == LZO_E_OK);
}

#define __prepare_tls_deflate __lzo_prepare_tls_deflate
#define __prepare_tls_inflate __lzo_prepare_tls_inflate
#define __destroy_tls_compress __lzo_destroy_tls
#define __destroy_tls_decompress __lzo_destroy_tls
#define __compress __lzo_compress
#define __compress2 __do_lzo_compress
#define __decompress __lzo_decompress

#endif


void
prepare_tls_compress(struct tls_compress * pcomp, int buffer_sz)
{
	assert(buffer_sz > 0);
	assert(pcomp != NULL);
	__prepare_tls_deflate(pcomp, buffer_sz);
}

void
destroy_tls_compress(struct tls_compress * pcomp)
{
	assert(pcomp != NULL);
	__destroy_tls_compress(pcomp);
}

void
prepare_tls_decompress(struct tls_compress * pcomp, int buffer_sz)
{
	assert(buffer_sz > 0);
	__prepare_tls_inflate(pcomp, buffer_sz);
}

void
destroy_tls_decompress(struct tls_compress * pcomp)
{
	__destroy_tls_decompress(pcomp);
}

void
compress(struct tls_compress * pcomp,
		const uint8_t * in_buf, int in_sz,
        uint8_t ** p_out_buf, unsigned int * p_out_sz)
{
	assert(pcomp != NULL);
	assert(in_buf != NULL);
	assert(in_sz >= 0);
	assert(p_out_buf != NULL);
	assert(p_out_sz != NULL);
	__compress(pcomp, in_buf, in_sz, p_out_buf, p_out_sz);
}

void
compress2(struct tls_compress * pcomp,
		const uint8_t * in_buf, int in_sz,
		uint8_t * out_buf, unsigned int * p_out_sz)
{
	assert(pcomp != NULL);
	assert(in_buf != NULL);
	assert(in_sz >= 0);
	assert(out_buf != NULL);
	__compress2(pcomp, in_buf, in_sz, out_buf, p_out_sz);
}

void
decompress(const uint8_t * in_buf, int in_sz,
		uint8_t * out_buf, int * out_sz)
{
	assert(in_buf != NULL);
	assert(in_sz >= 0);
	assert(out_buf != NULL);
	assert(out_sz != NULL);
	__decompress(in_buf, in_sz, out_buf, out_sz);
}

// vim:ts=4:sw=4

