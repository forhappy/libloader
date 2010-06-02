/* 
 * uncompress_log.c
 * by WN @ Jun 1, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>

#include <host/exception.h>
#include <host/mm.h>
#include <interp/logger.h>
#include <lzo/lzo.h>

#include <stdio.h>
#include <stdlib.h>

static void
usage(const char * argv0)
{
	printf("usage:\n");
	printf("%s <logfile> <output>\n", argv0);
	exit(1);
}

static void
read_from_file(FILE * fp, void * data, size_t sz)
{
	int err = fread(data, sz, 1, fp);
	if (err != 1) {
		if (err != 0)
			THROW_FATAL(EXP_FILE_READ_ERROR, "read input file error");
		else
			THROW(EXP_FILE_END, "file is end");
	}
}

static void
write_to_file(FILE * fp, void * data, size_t sz)
{
	int err = fwrite(data, sz, 1, fp);
	if (err != 1)
		THROW_FATAL(EXP_FILE_READ_ERROR, "read input file error");
}

/* if return TRUE, the file is end. */
static bool_t
read_mark(FILE * fp, uint8_t * pm)
{
	bool_t end = FALSE;
	define_exp(exp);
	TRY(exp) {
		read_from_file(fp, pm, sizeof(*pm));
	} FINALLY {
	} CATCH(exp) {
		switch (exp.type) {
		case EXP_FILE_END:
			end = TRUE;
			break;
		default:
			RETHROW(exp);
		}
	}
	return end;
}

static void
do_uncompress(FILE * in_fp, FILE * out_fp)
{
	uint8_t mark;
	
	catch_var(void *, src_buffer, NULL);
	size_t src_buffer_sz = 0;
	catch_var(void *, dest_buffer, NULL);
	size_t dest_buffer_sz = 0;


	define_exp(exp);
	TRY(exp) {
	
		if (read_mark(in_fp, &mark))
			THROW_FATAL(EXP_FILE_CORRUPTED, "unable to get first mark");

		while ((mark == COMPRESSED_DATA_MARK) ||
				(mark == UNCOMPRESSED_DATA_MARK))
		{
			switch (mark) {
			case COMPRESSED_DATA_MARK: {
				struct log_block_tag tag = {
					.real_sz = 0,
					.compressed_sz = 0,
				};
				/* read tag */
				read_from_file(in_fp, &tag, sizeof(tag));

				/* alloc buffers */
				if (src_buffer_sz < tag.compressed_sz) {
					set_catched_var(src_buffer,
							xrealloc(src_buffer, tag.compressed_sz));
					src_buffer_sz = tag.compressed_sz;
				}

				if (dest_buffer_sz < tag.real_sz) {
					set_catched_var(dest_buffer,
							xrealloc(dest_buffer, tag.real_sz));
					dest_buffer_sz = tag.real_sz;
				}

				read_from_file(in_fp, src_buffer, tag.compressed_sz);

				/* uncompress */
				size_t l = dest_buffer_sz;
				lzo1x_decompress_safe(src_buffer, tag.compressed_sz,
						dest_buffer, &l);
				if (l != tag.real_sz)
					THROW_FATAL(EXP_LZO_INFLATE, "deflate size error: should be %d, real is %d",
							tag.real_sz, l);

				/* write */
				write_to_file(out_fp, dest_buffer, l);

				break;
			}
			case UNCOMPRESSED_DATA_MARK: {
				/* read size */
				size_t size;
				read_from_file(in_fp, &size, sizeof(size));

				/* alloc buffers */
				if (dest_buffer_sz < size) {
					set_catched_var(dest_buffer,
							xrealloc(dest_buffer, size));
					dest_buffer_sz = size;
				}

				read_from_file(in_fp, dest_buffer, size);
				write_to_file(out_fp, dest_buffer, size);

				break;
			}
			default:
				THROW_FATAL(EXP_FILE_CORRUPTED, "logger file corrupted");
			}

			if (read_mark(in_fp, &mark)) {
				mark = UNCOMPRESSED_DATA_MARK;
				break;
			}
		}

		if ((mark != UNCOMPRESSED_DATA_MARK) &&
				(mark != COMPRESSED_DATA_MARK))
		{
			THROW_FATAL(EXP_FILE_CORRUPTED, "wrong mark 0x%x", mark);
		}

	} FINALLY {
		get_catched_var(src_buffer);
		get_catched_var(dest_buffer);
		if (src_buffer != NULL)
			free(src_buffer);
		if (dest_buffer != NULL)
			free(dest_buffer);
	} NO_CATCH(exp);
}
	

int
main(int argc, char * argv[])
{
	if (argc < 3)
		usage(argv[0]);

	FILE * in_fp = fopen(argv[1], "rb");
	if (in_fp == NULL)
		usage(argv[0]);

	FILE * out_fp = fopen(argv[2], "wb");
	if (out_fp == NULL) {
		fclose(in_fp);
		usage(argv[0]);
	}

	define_exp(exp);
	TRY(exp) {
		do_uncompress(in_fp, out_fp);
	} FINALLY {
		fclose(in_fp);
		fclose(out_fp);
	} NO_CATCH(exp);

	return 0;
}

// vim:ts=4:sw=4

