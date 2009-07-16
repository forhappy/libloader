/* 
 * ckptutils.c
 * by WN @ Jul. 16, 2009
 */

#include <errno.h>
#include <string.h>

#include "debug.h"
#include "exception.h"
#include "ckptutils.h"

static void
read_obj(void * obj, int sz_obj, FILE * fp)
{
	int err;
	err = fread(obj, sz_obj, 1, fp);
	if (err < 1)
		THROW(EXCEPTION_FATAL, "no more data in file");
	return;
}

static uint32_t
read_uint32(FILE * fp)
{
	uint32_t retval;
	read_obj(&retval, sizeof(retval), fp);
	return retval;
}

void
ckpt_clup(struct cleanup * clup)
{
	remove_cleanup(clup);
	struct ckpt_file * s = container_of(clup, struct ckpt_file,
			clup);
	if (s->fp)
		fclose(s->fp);
	if (s->nr_regions > 0) {
		if (s->regions) {
			for (int i = 0; i < s->nr_regions; i++) {
				if (s->regions[i] != NULL) {
					free(s->regions[i]);
					s->regions[i] = NULL;
				}
			}
			free(s->regions);
			s->regions = NULL;
		}
	}
}

struct ckpt_file *
load_ckpt_file(char * fn)
{
	struct ckpt_file * s = calloc(1, sizeof(*s));
	assert(s != NULL);
	s->clup.function = ckpt_clup;

	make_cleanup(&s->clup);

	errno = 0;
	FILE * fp = s->fp = fopen(fn, "rb");
	ETHROW("open ckpt file %s failed: %s", fn, strerror(errno));

	/* check magic */
	uint32_t magic;
	magic = read_uint32(fp);
	if (magic != CKPT_MAGIC)
		THROW(EXCEPTION_FATAL, "magic mismatch");

	/* first, build memory map */
	uint32_t sz_m = read_uint32(fp);
	int nr_regions = 0;
	while (sz_m != 0) {
		nr_regions ++;
		SYS_TRACE("read a mem region %d, sz=%d\n", nr_regions, sz_m);

		/* extend the regions array */
		s->regions = realloc(s->regions, nr_regions * sizeof(*s->regions));
		assert(s->regions != NULL);

		/* alloc mr */
		struct mem_region * mr;
		mr = calloc(sz_m, 1);
		assert(mr != NULL);
		s->regions[nr_regions - 1] = mr;

		/* fill mr */
		read_obj(mr, sz_m, fp);
		SYS_TRACE("mr %d:\n", nr_regions - 1);
		SYS_TRACE("\tstart : 0x%x\n", mr->start);
		SYS_TRACE("\tend   : 0x%x\n", mr->end);
		SYS_TRACE("\tprot  : 0x%x\n", mr->prot);
		SYS_TRACE("\toffset: 0x%x\n", mr->offset);
		SYS_TRACE("\tf_pos : 0x%x\n", mr->f_pos);
		SYS_TRACE("\tfn_len: %d\n", mr->fn_len);
		if (mr->fn_len != 0)
			SYS_TRACE("\tfn    :%s\n", mr->fn);

		/* skip real memory */
		fseek(fp, mr->end - mr->start, SEEK_CUR);

		/* read another sz */
		sz_m = read_uint32(fp);
	}
	s->nr_regions = nr_regions;

	/* load state vector */
	read_obj(&s->state, sizeof(s->state), fp);

	/* load regs */
	read_obj(&s->regs, sizeof(s->regs), fp);
	return s;
}


void
close_ckpt_file(struct ckpt_file * f)
{
	ckpt_clup(&f->clup);
}

