/* 
 * ckptutils.c
 * by WN @ Jul. 16, 2009
 */

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "debug.h"
#include "exception.h"
#include "ckptutils.h"
#include "checkpoint/checkpoint.h"

static void
ckpt_clup(struct cleanup * clup)
{
	remove_cleanup(clup);
	struct ckpt_file * s = container_of(clup, struct ckpt_file,
			clup);
	if (s->regions != NULL) {
		free(s->regions);
	}
	if (s->cmdline != NULL) {
		free(s->cmdline);
	}
	if (s->environ != NULL) {
		free(s->environ);
	}
	free(s);
}

static char *
build_strlst(char *** array, char * ptr)
{
	int len;
	int nr_items = 0;

	do {
		nr_items ++;
		*array = realloc(*array, nr_items * sizeof(char*));
		(*array)[nr_items - 1] = ptr;
		len = strlen(ptr);
		ptr += len + 1;
	} while (len != 0);
	/* set the last item to NULL */
	(*array)[nr_items - 1] = NULL;
	return ptr;
}

static void *
build_cmd_env(struct ckpt_file * s, void * cmdline)
{

	char * ptr = cmdline;

	ptr = build_strlst(&(s->cmdline), ptr);
	ptr = build_strlst(&(s->environ), ptr);

	return ptr;
}

struct ckpt_file *
load_ckpt_file(char * fn)
{
	struct ckpt_file * s = calloc(1, sizeof(*s));
	assert(s != NULL);
	s->clup.function = ckpt_clup;

	make_cleanup(&s->clup);

	errno = 0;

	/* map the ckpt file */
	int fd;
	errno = 0;
	fd = open(fn, O_RDONLY);
	ETHROW("open %s failed: %s", fn, strerror(errno));

	struct stat st;
	fstat(fd, &st);
	ETHROW("fstat failed: %s", strerror(errno));

	void * ckpt_img;
	ckpt_img = mmap(NULL, st.st_size, PROT_READ,
			MAP_PRIVATE, fd, 0);
	ETHROW("mmap failed: %s", strerror(errno));
	close(fd);

	s->ckpt_img = ckpt_img;

	/* check magic */
	if (*(uint32_t*)ckpt_img != CKPT_MAGIC)
		THROW(EXCEPTION_FATAL, "magic mismatch");

	/* build cmdline */
	void * p;
	p = build_cmd_env(s, ckpt_img + sizeof(CKPT_MAGIC));

	/* set the state */
	s->state = p;
	p += sizeof(*(s->state));

	/* load mem regions */
	uint32_t sz_m = 0;
	s->nr_regions = 0;
	sz_m = *(uint32_t *)p;
	p += sizeof(sz_m);
	while (sz_m != 0) {
		s->nr_regions ++;
		s->regions = realloc(s->regions,
				s->nr_regions * sizeof(*s->regions));
		struct mem_region * mr = p;

		SYS_TRACE("region %d, start: 0x%x, end: 0x%x (p-img=0x%x)\n",
				s->nr_regions, mr->start, mr->end, p - ckpt_img);

		s->regions[s->nr_regions - 1] = mr;
		p += sz_m;
		p += mr->end - mr->start;
		sz_m = *(uint32_t *)p;
		p += sizeof(sz_m);
	}
	return s;
}


void
close_ckpt_file(struct ckpt_file * f)
{
	ckpt_clup(&f->clup);
}

