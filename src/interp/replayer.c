/* 
 * replayer.c
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <common/debug.h>

#include <xasm/utils.h>
#include <xasm/processor.h>
#include <xasm/syscall.h>
#include <xasm/string.h>
#include <xasm/types_helper.h>
#include <xasm/tls.h>

#include <interp/checkpoint.h>

static int ckpt_fd = 0;
static struct checkpoint_head ckpt_head;

static void
read_from_file(int fd, void * buffer, size_t sz)
{
	int err;
	err = INTERNAL_SYSCALL_int80(read, 3, fd, buffer, sz);
	CASSERT(REPLAYER, err == (int)sz, "read error: %d\n", err);
}

static void
skip_nbytes(int fd, size_t n)
{
	int err;
	unsigned long long cur_pos ATTR_UNUSED;
	err = INTERNAL_SYSCALL_int80(_llseek, 5, fd,
			0, n, &cur_pos, SEEK_CUR);
	CASSERT(REPLAYER, err >= 0, "_llseek failed: %d\n", err);
}

static void
read_head(struct checkpoint_head * head, int fd)
{
	TRACE(REPLAYER, "read ckpt head\n");
	read_from_file(fd, head, sizeof(*head));

	/* head checker */
	if (memcmp(head->magic, CKPT_MAGIC, CKPT_MAGIC_SZ) != 0)
		FATAL(REPLAYER, "checkpoint magic error\n");
	TRACE(REPLAYER, "checkpoint head magic passed\n");
	TRACE(REPLAYER, "pid=%d, tid=%d, tnr=%d\n", head->pid, head->tid, head->tnr);
}

#if 0

/* if return FALSE, the regions end */
/* we use a function here because we want to use alloca */
static bool_t
restore_memory_region(int fd, const char * interp_fn, const char * exec_fn,
		const char * pthread_fn)
{
	struct mem_region region;
	read_from_file(fd, &region, sizeof(region));

	if (region.start == MEM_REGIONS_END_MARK)
		return FALSE;

	assert(region.fn_sz > 0);
	char * fn = alloca(region.fn_sz);
	read_from_file(fd, fn, region.fn_sz);
	TRACE(REPLAYER, "restoring region 0x%x-0x%x:%1x:0x%x:%d:%s\n",
			region.start, region.end, region.prot, region.offset,
			region.fn_sz, fn);
#if 0
	/* FIXME */
	INTERNAL_SYSCALL_int80(lseek, 3, fd, region_sz(&region), SEEK_CUR);
#endif




#if 0
	if ((fn[0] != '\0') && (fn[0] != '[')) {
		/* this is file mapping */
		/* if map from a device, create a anonymouse mapping for it */
		bool_t anon = FALSE;
		if (strncmp("/dev", fn, 4) == 0) {
			anon = TRUE;
		}

	} else {
		/* this is anonymouse mapping */
	}
#endif
	return TRUE;
}

static void
restore_memory(int fd, const char * interp_fn, const char * exec_fn,
		const char * pthread_fn)
{
	TRACE(REPLAYER, "restoring memory image\n");
	while (restore_memory_region(fd, interp_fn, exec_fn, pthread_fn));
}

#endif
#if 0
/* if return true, continue the iteration.
 * if return false, stop the iteration */
/* we use a function because we want to use alloca */
/* handler returns false then interrupt the iteration */
static bool_t
process_next_region(bool_t (*handler)(struct mem_region *, const char *, intptr_t),
		intptr_t arg)
{
	struct mem_region region;
	read_from_file(ckpt_fd, &region, sizeof(region));

	if (region.start == MEM_REGIONS_END_MARK)
		return FALSE;

	assert(region.fn_sz > 0);
	char * fn = alloca(region.fn_sz);
	read_from_file(fd, fn, region.fn_sz);
	TRACE(REPLAYER, "processing region 0x%x-0x%x:%1x:0x%x:%d:%s\n",
			region.start, region.end, region.prot, region.offset,
			region.fn_sz, fn);

	if (!handler(&region, fn, arg))
		return FALSE;
	return TRUE;
}

static bool_t
check_restore_tls_stack(struct mem_region * r, const char * fn ATTR_UNUSED,
		intptr_t arg)
{
	int tnr = arg;
	void * stack_base = TNR_TO_STACK(tnr);
	void * stack_end = stack_base + TLS_STACK_SIZE;
	if ((r->start == (uintptr_t)stack_base) &&
		(r->end == (uintptr_t)stack_end)) {
		/* find it! */
		return FALSE;
	}
}
#endif

static void
do_restore_tls_stack(void)
{
	int tnr = ckpt_head.tnr;
	replay_init_tls(tnr);

	void * stack_base = TNR_TO_STACK(tnr) + GUARDER_LENGTH;
	void * stack_end = stack_base + TLS_STACK_SIZE - GUARDER_LENGTH;

	while (TRUE) {
		struct mem_region region;
		read_from_file(ckpt_fd, &region, sizeof(region));

		if (region.start == MEM_REGIONS_END_MARK)
			FATAL(REPLAYER, "unable to find tls stack in ckpt\n");

		skip_nbytes(ckpt_fd, region.fn_sz);
		TRACE(REPLAYER, "compare: 0x%8x-0x%8x %8x-%8x\n", region.start,
				region.end, (uintptr_t)stack_base, (uintptr_t)stack_end);
		if ((region.start == (uintptr_t)stack_base) &&
				(region.end == (uintptr_t)stack_end)) {
			/* we find it! */
			read_from_file(ckpt_fd, stack_base, TLS_STACK_SIZE -
					GUARDER_LENGTH);
			break;
		}
		skip_nbytes(ckpt_fd, region_sz(&region));
	}

	TRACE(REPLAYER, "tls stack is restored\n");
}

__attribute__((used, unused, visibility("hidden"))) void
restore_tls_stack(
		const char * interp_fn ATTR_UNUSED,
		const char * exec_fn ATTR_UNUSED,
		const char * ckpt_fn ATTR_UNUSED,
		const char * pthread_fn ATTR_UNUSED,
		void * p__stack_user ATTR_UNUSED,
		void * pstack_used ATTR_UNUSED)
{
	relocate_interp();
	VERBOSE(REPLAYER, "interp_fn: %s\n", interp_fn);
	VERBOSE(REPLAYER, "exec_fn: %s\n", exec_fn);
	VERBOSE(REPLAYER, "ckpt_fn: %s\n", ckpt_fn);
	VERBOSE(REPLAYER, "pthread_fn: %s\n", pthread_fn);
	VERBOSE(REPLAYER, "p__stack_user: %p\n", p__stack_user);
	VERBOSE(REPLAYER, "pstack_used: %p\n", pstack_used);

	ckpt_fd = INTERNAL_SYSCALL_int80(open, 2,
			ckpt_fn, O_RDONLY);
	CASSERT(REPLAYER, ckpt_fd > 0, "open %s failed: %d\n", ckpt_fn, ckpt_fd);

	read_head(&ckpt_head, ckpt_fd);

	/* restore tls */
	int offset = INTERNAL_SYSCALL_int80(lseek, 3, ckpt_fd, 0, SEEK_CUR);
	CASSERT(REPLAYER, offset >= 0, "lseek failed\n");

	do_restore_tls_stack();

	int err = INTERNAL_SYSCALL_int80(lseek, 3, ckpt_fd, offset, SEEK_SET);
	CASSERT(REPLAYER, err >= 0, "revert file pos failed\n");
}

static void
do_restore_mem_region(struct mem_region * region,
		const char * interp_fn, const char * exec_fn,
		const char * ckpt_fn, const char * pthread_fn)
{
	char * fn = alloca(region->fn_sz);
	read_from_file(ckpt_fd, fn, region->fn_sz);
	TRACE(REPLAYER, "restoring 0x%8x-0x%8x:%s\n", region->start, region->end,
			fn);

	struct thread_private_data * tpd = get_tpd();
	if (region->start == TNR_TO_STACK(tpd->tnr) + GUARDER_LENGTH) {
		assert(region->end == TNR_TO_STACK(tpd->tnr) +
				GUARDER_LENGTH + TLS_STACK_SIZE);
		return;
	}

	bool_t anon_mapping = FALSE;
	bool_t file_mapping = TRUE;
	bool_t do_fill = TRUE;
	if ((fn[0] != '\0') && (fn[0] != '[')) {
		/* this is file mapping */
		if (strncmp("/dev", fn, 4) == 0) {
			anon_mapping = TRUE;
			file_mapping = FALSE;
			do_fill = FALSE;
		} else if (strcmp(exec_fn, fn) == 0) {
			/* don't do the file mapping,  */
			anon_mapping = FALSE;
			file_mapping = FALSE;
			do_fill = TRUE;
		} else if (strcmp(pthread_fn, fn) == 0) {
			anon_mapping = TRUE;
			file_mapping = FALSE;
			do_fill = TRUE;
		} else {
			anon_mapping = FALSE;
			file_mapping = TRUE;
			do_fill = TRUE;
		}
	} else {
	}

	/* FIXME */
	/* 0. if it is TNR_TO_STACK + GUARDER_LENGTH, skip this section
	 * 1. setup the region:
	 *   if it is file mapping, then
	 *     if it is mapped from a device file, use anon mapping;
	 *     if it is mapped from exec_fn, then it should have been mapped,
	 *        do nothing
	 *     if it is mapped from pthread_fn, use anon mapping,
	 *     if it is mapped from normal file, do the file mapping
	 *   if it is anon mapping, then
	 *     if it is interp's own data section, skip;
	 *     if it is stack, skip;
	 *     if it is heap?
	 * 2. fill its content
	 *   if it is mapped from "/dev/xxx", fill it with zero
	 *   if not: 
	 *   don't fill interp(myself)'s own data section. We can find their
	 *   address from _end and _edata
	 * */
	skip_nbytes(ckpt_fd, region_sz(region));
}

__attribute__((used, unused, visibility("hidden"))) void
replayer_main(volatile struct pusha_regs pusha_regs)
{

	struct thread_private_data * tpd = get_tpd();

	void ** args = tpd->old_stack_top;
	const char * interp_fn = args[0];
	const char * exec_fn = args[1];
	const char * ckpt_fn = args[2];
	const char * pthread_fn = args[3];
	void * p__stack_user = args[4];
	void * pstack_used = args[5];

	VERBOSE(REPLAYER, "interp_fn: %s\n", interp_fn);
	VERBOSE(REPLAYER, "exec_fn: %s\n", exec_fn);
	VERBOSE(REPLAYER, "ckpt_fn: %s\n", ckpt_fn);
	VERBOSE(REPLAYER, "pthread_fn: %s\n", pthread_fn);
	VERBOSE(REPLAYER, "p__stack_user: %p\n", p__stack_user);
	VERBOSE(REPLAYER, "pstack_used: %p\n", pstack_used);

	/* load each memory region */
	struct mem_region region;
	do {
		read_from_file(ckpt_fd, &region, sizeof(region));
		if (region.start != MEM_REGIONS_END_MARK) {
			do_restore_mem_region(&region, interp_fn, exec_fn, ckpt_fn,
					pthread_fn);
		}
	} while (region.start != MEM_REGIONS_END_MARK);

	INTERNAL_SYSCALL_int80(close, 1, ckpt_fd);
	FATAL(REPLAYER, "Quit\n");
}
// vim:ts=4:sw=4

