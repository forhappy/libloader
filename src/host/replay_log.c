/* 
 * replay_log.c
 * by WN @ Jun. 21, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <host/replay_log.h>
#include <host/exception.h>
#include <host/mm.h>

#include <interp/logger.h>
#include <lzo/lzo.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static FILE * log_fp = NULL;

static size_t uncomp_data_buffer_sz = 0;
static void * uncomp_data_buffer = NULL;
static void * uncomp_data_buffer_ptr = NULL;

static void
read_from_file(void * data, size_t sz)
{
	int err = fread(data, sz, 1, log_fp);
	if (err != 1) {
		if (err != 0)
			THROW_FATAL(EXP_LOG_CORRUPTED, "read input file error");
		else
			THROW(EXP_LOG_END, "file is end");
	}
}

static uint8_t
read_mark(void)
{
	uint8_t r;
	read_from_file(&r, sizeof(r));
	return r;
}

void
open_log(const char * fn)
{
	if (log_fp != NULL)
		FATAL(REPLAYER_HOST, "call open_log before close it\n");
	log_fp = fopen(fn, "rb");
	if (log_fp == NULL)
		THROW_FATAL(EXP_FILE_NOT_FOUND, "unable to fopen log file %s", fn);
}

static void
free_buffers(void)
{
	xfree_null(uncomp_data_buffer);
	uncomp_data_buffer_sz = 0;
	uncomp_data_buffer_ptr = NULL;
}

void
close_log(void)
{
	free_buffers();
	if (log_fp != NULL)
		fclose(log_fp);
}

size_t
read_log(void * buffer, size_t size)
{
	assert(buffer != NULL);
	if (size == 0)
		return 0;

	if (uncomp_data_buffer_ptr + size <=
			uncomp_data_buffer + uncomp_data_buffer_sz) {
		memcpy(buffer, uncomp_data_buffer_ptr, size);
		uncomp_data_buffer_ptr += size;
		return size;
	}

	/* read the last bytes */
	size_t last_sz = uncomp_data_buffer +
		uncomp_data_buffer_sz -
		uncomp_data_buffer_ptr;
	if (last_sz != 0) {
		assert(size > last_sz);
		memcpy(buffer, uncomp_data_buffer_ptr, last_sz);
		buffer += last_sz;
		uncomp_data_buffer_ptr += last_sz;
		size -= last_sz;
		return last_sz;
	}

	free_buffers();

	/* read mark */
	uint8_t mark = read_mark();
	if ((mark != COMPRESSED_DATA_MARK) &&
			(mark != UNCOMPRESSED_DATA_MARK))
		THROW_FATAL(EXP_FILE_CORRUPTED, "wrong mark: 0x%x", mark);

	if (mark == COMPRESSED_DATA_MARK) {
		struct log_block_tag tag = {
			.real_sz = 0,
			.compressed_sz = 0,
		};
		/* read tag */
		read_from_file(&tag, sizeof(tag));

		catch_var(void *, ori_buf, NULL);
		catch_var(void *, uncomp_buf, NULL);
		define_exp(exp);
		TRY(exp) {

			set_catched_var(ori_buf, xmalloc(tag.compressed_sz));
			set_catched_var(uncomp_buf, xmalloc(tag.real_sz));

			read_from_file(ori_buf, tag.compressed_sz);

			/* uncompress */
			size_t l = tag.real_sz;
			lzo1x_decompress_safe(ori_buf, tag.compressed_sz,
					uncomp_buf, &l);
			if (l != tag.real_sz)
				THROW_FATAL(EXP_LZO_INFLATE, "deflate size error: should be %d, real is %d",
						tag.real_sz, l);
		} FINALLY {
			get_catched_var(ori_buf);
			xfree_null(ori_buf);
		} CATCH(exp) {
			get_catched_var(uncomp_buf);
			xfree_null(uncomp_buf);
			RETHROW(exp);
		}

		assert(uncomp_buf != NULL);
		uncomp_data_buffer = uncomp_buf;
		uncomp_data_buffer_sz = tag.real_sz;
		uncomp_data_buffer_ptr = uncomp_data_buffer;

	} else {
		/* uncompressed data */
		size_t size;
		read_from_file(&size, sizeof(size));

		uncomp_data_buffer = xmalloc(size);
		assert(uncomp_data_buffer != NULL);

		read_from_file(uncomp_data_buffer, size);
		uncomp_data_buffer_ptr = uncomp_data_buffer;
		uncomp_data_buffer_sz = size;
	}

	return 0;
}

void
read_log_full(void * buf, size_t size)
{
	bool_t last_zero = FALSE;
	assert(buf != NULL);
	while (size > 0) {
		size_t read_sz = read_log(buf, size);
		if (read_sz == 0) {
			if (last_zero)
				THROW_FATAL(EXP_LOG_CORRUPTED,
						"unable to read %d data from log", size);
			last_zero = TRUE;
			continue;
		}

		last_zero = FALSE;
		buf += read_sz;
		size -= read_sz;
	}
}

#define BUFFER_SZ	(1024 * 1024)

void
uncompress_log(const char * log_fn, const char * out_fn)
{
	assert(log_fn != NULL);
	assert(out_fn != NULL);

	open_log(log_fn);

	catch_var(FILE *, fp, NULL);
	catch_var(void *, buffer, NULL);
	define_exp(exp);
	TRY(exp) {

		set_catched_var(fp, fopen(out_fn, "wb"));
		if (fp == NULL)
			THROW_FATAL(EXP_FILE_NOT_FOUND, "unable to open file %s", out_fn);
		set_catched_var(buffer, xmalloc(BUFFER_SZ));

		while (TRUE) {
			size_t sz = read_log(buffer, BUFFER_SZ);
			if (sz != 0) {
				int err = fwrite(buffer, sz, 1, fp);
				if (err != 1)
					THROW_FATAL(EXP_FILE_WRITE_ERROR,
							"write output file error");
			}
		}

	} FINALLY {
		get_catched_var(fp);
		get_catched_var(buffer);

		close_log();
		if (fp != NULL)
			fclose(fp);
		xfree_null(buffer);
	} CATCH(exp) {
		switch (exp.type) {
		case EXP_LOG_END:
			VERBOSE(REPLAYER_HOST, "log is end\n");
			return;
		default:
			RETHROW(exp);
		}
	}
}

// vim:ts=4:sw=4

