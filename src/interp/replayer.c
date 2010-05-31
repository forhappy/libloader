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
	TRACE(REPLAYER, "skip %u bytes\n", n);
	err = INTERNAL_SYSCALL_int80(_llseek, 5, fd,
			0, n, &cur_pos, SEEK_CUR);
	TRACE(REPLAYER, "cur_pos = %llu, err=%d\n", cur_pos, err);
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

static void
do_restore_tls_stack(void)
{
	int tnr = ckpt_head.tnr;

	TRACE(REPLAYER, "tnr = %d\n", tnr);

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

/* make all exec section writable */
static int
get_new_prot(int old_prot)
{
	if (old_prot & PROT_EXEC)
		old_prot |= PROT_WRITE;
	return old_prot;
}


static void
fix_unwritable_region(struct mem_region * region, bool_t mark_writable)
{
	int err;
	int prot = get_new_prot(region->prot);
	if (prot & PROT_WRITE)
		return;
	/* all executable regions should have been set as 'writable' when mapping */

	if (mark_writable)
		prot |= PROT_WRITE;
	err = INTERNAL_SYSCALL_int80(mprotect, 3, region->start,
			region_sz(region), prot);
	assert(err == 0);
}

static void
do_file_mapping(struct mem_region * region, const char * fn)
{
	int fd = INTERNAL_SYSCALL_int80(open, 2, fn, O_RDONLY);
	int prot = get_new_prot(region->prot);
	int map_type = MAP_FIXED | MAP_PRIVATE;

	if (fd < 0) {
		WARNING(REPLAYER, "open file %s failed: %d\n", fn, fd);
		/* do anon mapping */
		map_type |= MAP_ANONYMOUS;
	}

	TRACE(REPLAYER, "doing file mapping: 0x%8x-0x%8x:%d\n",
			region->start, region->end, prot);
	void * addr = (void*)INTERNAL_SYSCALL_int80(mmap2, 6,
			region->start, region_sz(region), prot,
			map_type, fd, region->offset >> 12);
	assert(addr == (void*)region->start);
	if (fd >= 0)
		INTERNAL_SYSCALL_int80(close, 1, fd);
}

static void
do_anon_mapping(struct mem_region * region)
{
	int prot = get_new_prot(region->prot);
	int map_type = MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS;

	void * addr = (void*)INTERNAL_SYSCALL_int80(mmap2, 6,
			region->start, region_sz(region), prot,
			map_type, 0, 0);
	assert(addr == (void*)region->start);
}

static bool_t
do_reprotect(struct mem_region * region)
{
	int prot = get_new_prot(region->prot);
	int err = INTERNAL_SYSCALL_int80(mprotect, 3,
			region->start, region_sz(region), prot);
	if (err != 0) {
		if (err == -ENOMEM)
			return err;
		else
			FATAL(REPLAYER, "reprotect 0x%8x-0x%8x failed: %d\n",
				region->start, region->end, err);
	}
	return 0;
}

static void
do_restore_mem_region(struct mem_region * region,
		const char * interp_fn, const char * exec_fn,
		const char * pthread_fn)
{
	extern int _end[] ATTR_HIDDEN;
	char * fn = alloca(region->fn_sz);
	read_from_file(ckpt_fd, fn, region->fn_sz);
	TRACE(REPLAYER, "restoring 0x%8x-0x%8x:%s\n", region->start, region->end,
			fn);

	struct thread_private_data * tpd = get_tpd();
	if (region->start == (uintptr_t)(TNR_TO_STACK(tpd->tnr) +
				GUARDER_LENGTH)) {
		TRACE(REPLAYER, "this is tls stack\n");
		assert(region->end == (uintptr_t)(TNR_TO_STACK(tpd->tnr) +
					TLS_STACK_SIZE));
		skip_nbytes(ckpt_fd, region_sz(region));
		return;
	}

	/* if it is interp mapping, skip and return */
	if (strcmp(fn, interp_fn) == 0) {
		/* this is interp_fn mapping */
		skip_nbytes(ckpt_fd, region_sz(region));
		return;
	}

	/* FIXME */
	/* 0. if it is TNR_TO_STACK + GUARDER_LENGTH, skip this section
	 * 1. setup the region:
	 *   if it is file mapping, then
	 *     if it is mapped from a device file, use anon mapping, fill = false;
	 *     if it is mapped from exec_fn, then it should have been mapped,
	 *        we issue 'reprotect'. I've tried force-remap, it cause the lost
	 *        of /proc/xxx/exec.
	 *     if it is mapped from pthread_fn, use anon mapping,
	 *     if it is mapped from normal file, do the file mapping
	 *   if it is anon mapping, then
	 *     if it is interp's own data section, don't fill data before _end;
	 *     if it is tls-stack, skip;
	 * 2. fill its content
	 *   if it is mapped from "/dev/xxx", fill it with zero
	 *   if not: 
	 *   don't fill interp(myself)'s own data section. We can find their
	 *   address from _end and _edata
	 * */

	bool_t anon_mapping = FALSE;
	bool_t file_mapping = FALSE;
	bool_t reprotect = FALSE;
	bool_t do_fill = TRUE;

	if ((fn[0] != '\0') && (fn[0] != '[')) {
		/* the original section is file mapping */
		if (strncmp("/dev", fn, 4) == 0) {
			do_fill = FALSE;
			anon_mapping = TRUE;
			file_mapping = FALSE;
			reprotect = FALSE;
		} else if (strcmp(exec_fn, fn) == 0) {
			do_fill = TRUE;
			file_mapping = FALSE;
			anon_mapping = FALSE;
			/* see the previous discussing of reprotect */
			reprotect = TRUE;
		} else if (strcmp(pthread_fn, fn) == 0) {
			do_fill = TRUE;
			file_mapping = FALSE;
			anon_mapping = TRUE;
			reprotect = FALSE;
		} else {
			do_fill = TRUE;
			file_mapping = TRUE;
			anon_mapping = FALSE;
			reprotect = FALSE;
		}
		/* we have already skip libinterp.so mapping */
	} else {
		/* the original section is anon mapping */
		if ((region->start < (uintptr_t)_end) &&
				(region->end >= (uintptr_t)_end)) {
			/* check whether this section is the interp's data. if it is:
			 *   1. if this program is loaded by libinterp.so, this is the [heap]
			 *      section. we have already restore heap by 'brk' call in
			 *      replayer_main, so we needn't do any mapping.
			 *   2. if this program is loaded by itself, then the data section of
			 *      libinterp.so never expand.
			 * In short, we needn't to mapping for it, and when filling, we only need
			 * to fill those data after _end
			 * */
			do_fill = TRUE;
			file_mapping = FALSE;
			anon_mapping = FALSE;
			reprotect = FALSE;
		} else {
			do_fill = TRUE;
			file_mapping = FALSE;
			anon_mapping = TRUE;
			reprotect = FALSE;
		}
	}

	/* mapping */
	if (file_mapping) {
		TRACE(REPLAYER, "file mapping...\n");
		do_file_mapping(region, fn);
	} else if (anon_mapping) {
		TRACE(REPLAYER, "anon mapping...\n");
		do_anon_mapping(region);
	} else if (reprotect) {
		TRACE(REPLAYER, "reprotecting...\n");
		int err = do_reprotect(region);
		if (err) {
			TRACE(REPLAYER, "reprotect failed, use file mapping...\n");
			do_file_mapping(region, fn);
		}
	}

	if (do_fill) {
		fix_unwritable_region(region, TRUE);
		if ((region->start < (uintptr_t)_end) &&
				(region->end >= (uintptr_t)_end)) {
			TRACE(REPLAYER, "_end=%p\n", _end);

			skip_nbytes(ckpt_fd,
					region_sz(region) -
					ptr_diff(region->end, _end));
			read_from_file(ckpt_fd, _end,
					ptr_diff(region->end, _end));
		} else {
			TRACE(REPLAYER, "fill %d bytes\n", region_sz(region));
			read_from_file(ckpt_fd, (void*)region->start,
					region_sz(region));
		}
		fix_unwritable_region(region, FALSE);
	}
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

	/* restore brk */
	/* it will expand the heap and maps the pages */
	int err = INTERNAL_SYSCALL_int80(brk, 1, ckpt_head.brk);
	assert((uintptr_t)err == ckpt_head.brk);

	/* load each memory region */
	struct mem_region region;
	do {

		read_from_file(ckpt_fd, &region, sizeof(region));
		if (region.start != MEM_REGIONS_END_MARK) {
			do_restore_mem_region(&region, interp_fn, exec_fn,
					pthread_fn);
		}
	} while (region.start != MEM_REGIONS_END_MARK);

	INTERNAL_SYSCALL_int80(close, 1, ckpt_fd);

	/* restoring thread area */
	/* thread areas */
	TRACE(REPLAYER, "restoring thread area\n");
	for (int i = 0; i < GDT_ENTRY_TLS_ENTRIES; i++) {
		int err;
		err = INTERNAL_SYSCALL_int80(set_thread_area,
				1, &(ckpt_head.thread_area[i]));
		assert(err == 0);
	}

	/* restore register state */
	void * eip;
	struct pusha_regs * pregs = (struct pusha_regs *)(&pusha_regs);
	restore_reg_state(&ckpt_head.reg_state, pregs, &eip);


	/* prepare replay: */
	/* setup offset */
	tpd->target = eip;
	TRACE(REPLAYER, "target eip = %p\n", eip);

	/* setup function pointers */

#if 1
	volatile int i = 0;

	pid_t self_pid = INTERNAL_SYSCALL_int80(getpid, 0);

	VERBOSE(REPLAYER, "state has been restored, run gdb:\n");
	VERBOSE(REPLAYER, "\t(gdb) attach %d\n", self_pid);
	VERBOSE(REPLAYER, "\t(gdb) p *(int*)(%p) = 1\n", &i);
	while (i == 0) {
		struct timespec {
			long       ts_sec;
			long       ts_nsec;
		};
		struct timespec tm = {1, 0};
		INTERNAL_SYSCALL_int80(nanosleep, 2, &tm, NULL);
	}
#endif
}

// vim:ts=4:sw=4
