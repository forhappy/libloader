/* 
 * replayer.c
 * by WN @ Apr. 26, 2010
 */

#include <common/defs.h>
#include <common/debug.h>
#include <common/replay/socketpair.h>

#include <xasm/utils.h>
#include <xasm/processor.h>
#include <xasm/syscall.h>
#include <xasm/string.h>
#include <xasm/types_helper.h>
#include <xasm/tls.h>

#include <interp/checkpoint.h>

static int ckpt_fd = 0;
static struct checkpoint_head ckpt_head;
static void * stack_base = NULL;
static void * stack_end = NULL;

static void
read_from_file(int fd, void * buffer, size_t sz)
{
	int err;
	err = INTERNAL_SYSCALL_int80(read, 3, fd, buffer, sz);
	CASSERT(REPLAYER_TARGET, err == (int)sz, "read error: %d\n", err);
}

static void
skip_nbytes(int fd, size_t n)
{
	int err;
	unsigned long long cur_pos ATTR_UNUSED;
	TRACE(REPLAYER_TARGET, "skip %u bytes\n", n);
	err = INTERNAL_SYSCALL_int80(_llseek, 5, fd,
			0, n, &cur_pos, SEEK_CUR);
	TRACE(REPLAYER_TARGET, "cur_pos = %llu, err=%d\n", cur_pos, err);
	CASSERT(REPLAYER_TARGET, err >= 0, "_llseek failed: %d\n", err);
}

static void
read_head(struct checkpoint_head * head, int fd)
{
	TRACE(REPLAYER_TARGET, "read ckpt head\n");
	read_from_file(fd, head, sizeof(*head));

	/* head checker */
	if (memcmp(head->magic, CKPT_MAGIC, CKPT_MAGIC_SZ) != 0)
		FATAL(REPLAYER_TARGET, "checkpoint magic error\n");
	TRACE(REPLAYER_TARGET, "checkpoint head magic passed\n");
	TRACE(REPLAYER_TARGET, "pid=%d, tid=%d, tnr=%d\n", head->pid, head->tid, head->tnr);
}

static void
do_restore_tls_stack(void)
{
	int tnr = ckpt_head.tnr;

	TRACE(REPLAYER_TARGET, "tnr = %d\n", tnr);

	replay_init_tls(tnr);

	stack_base = TNR_TO_STACK(tnr);
	stack_end = stack_base + TLS_STACK_SIZE;
	void * real_stack_base = stack_base + GUARDER_LENGTH;

	while (TRUE) {
		struct mem_region region;
		read_from_file(ckpt_fd, &region, sizeof(region));

		if (region.start == MEM_REGIONS_END_MARK)
			FATAL(REPLAYER_TARGET, "unable to find tls stack in ckpt\n");

		skip_nbytes(ckpt_fd, region.fn_sz);
		TRACE(REPLAYER_TARGET, "compare: 0x%8x-0x%8x %8x-%8x\n", region.start,
				region.end, (uintptr_t)real_stack_base,
				(uintptr_t)stack_end);
		if ((region.start == (uintptr_t)real_stack_base) &&
				(region.end == (uintptr_t)stack_end)) {
			/* we find it! */
			read_from_file(ckpt_fd, real_stack_base, TLS_STACK_SIZE -
					GUARDER_LENGTH);
			break;
		}
		skip_nbytes(ckpt_fd, region_sz(&region));
	}

	TRACE(REPLAYER_TARGET, "tls stack is restored\n");
}

__attribute__((used, unused, visibility("hidden"))) void
restore_tls_stack(
		const char * interp_fn ATTR_UNUSED,
		const char * exec_fn ATTR_UNUSED,
		const char * ckpt_fn ATTR_UNUSED)
{
	relocate_interp();
	VERBOSE(REPLAYER_TARGET, "interp_fn: %s\n", interp_fn);
	VERBOSE(REPLAYER_TARGET, "exec_fn: %s\n", exec_fn);
	VERBOSE(REPLAYER_TARGET, "ckpt_fn: %s\n", ckpt_fn);

	ckpt_fd = INTERNAL_SYSCALL_int80(open, 2,
			ckpt_fn, O_RDONLY);
	CASSERT(REPLAYER_TARGET, ckpt_fd > 0, "open %s failed: %d\n", ckpt_fn, ckpt_fd);

	read_head(&ckpt_head, ckpt_fd);

	/* restore tls */
	int offset = INTERNAL_SYSCALL_int80(lseek, 3, ckpt_fd, 0, SEEK_CUR);
	CASSERT(REPLAYER_TARGET, offset >= 0, "lseek failed\n");

	do_restore_tls_stack();

	int err = INTERNAL_SYSCALL_int80(lseek, 3, ckpt_fd, offset, SEEK_SET);
	CASSERT(REPLAYER_TARGET, err >= 0, "revert file pos failed\n");
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
		WARNING(REPLAYER_TARGET, "open file %s failed: %d\n", fn, fd);
		/* do anon mapping */
		map_type |= MAP_ANONYMOUS;
	}

	TRACE(REPLAYER_TARGET, "doing file mapping: 0x%8x-0x%8x:%d\n",
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
			FATAL(REPLAYER_TARGET, "reprotect 0x%8x-0x%8x failed: %d\n",
				region->start, region->end, err);
	}
	return 0;
}

static void
do_restore_mem_region(struct mem_region * region,
		const char * interp_fn, const char * exec_fn)
{
	extern int _end[] ATTR_HIDDEN;
	char * fn = alloca(region->fn_sz);
	read_from_file(ckpt_fd, fn, region->fn_sz);
	TRACE(REPLAYER_TARGET, "restoring 0x%8x-0x%8x:%s\n", region->start, region->end,
			fn);

	struct thread_private_data * tpd = get_tpd();
	if (region->start == (uintptr_t)(TNR_TO_STACK(tpd->tnr) +
				GUARDER_LENGTH)) {
		TRACE(REPLAYER_TARGET, "this is tls stack\n");
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
		TRACE(REPLAYER_TARGET, "file mapping...\n");
		do_file_mapping(region, fn);
	} else if (anon_mapping) {
		TRACE(REPLAYER_TARGET, "anon mapping...\n");
		do_anon_mapping(region);
	} else if (reprotect) {
		TRACE(REPLAYER_TARGET, "reprotecting...\n");
		int err = do_reprotect(region);
		if (err) {
			TRACE(REPLAYER_TARGET, "reprotect failed, use file mapping...\n");
			do_file_mapping(region, fn);
		}
	}

	if (do_fill) {
		fix_unwritable_region(region, TRUE);
		if ((region->start < (uintptr_t)_end) &&
				(region->end >= (uintptr_t)_end)) {
			TRACE(REPLAYER_TARGET, "_end=%p\n", _end);

			skip_nbytes(ckpt_fd,
					region_sz(region) -
					ptr_diff(region->end, _end));
			read_from_file(ckpt_fd, _end,
					ptr_diff(region->end, _end));
		} else {
			TRACE(REPLAYER_TARGET, "fill %d bytes\n", region_sz(region));
			read_from_file(ckpt_fd, (void*)region->start,
					region_sz(region));
		}
		fix_unwritable_region(region, FALSE);
	}
}

static void
wait_for_attach(void)
{
#if 0
	volatile int i = 0;

	pid_t self_pid = INTERNAL_SYSCALL_int80(getpid, 0);

	VERBOSE(REPLAYER_TARGET, "state has been restored, run gdb:\n");
	VERBOSE(REPLAYER_TARGET, "\t(gdb) attach %d\n", self_pid);
	VERBOSE(REPLAYER_TARGET, "\t(gdb) p *(int*)(%p) = 1\n", &i);
	while (i == 0) {
		struct timespec {
			long       ts_sec;
			long       ts_nsec;
		};
		struct timespec tm = {1, 0};
		INTERNAL_SYSCALL_int80(nanosleep, 2, &tm, NULL);
	}
#endif

	/* kill myself using a sigstop */
	pid_t self_pid = INTERNAL_SYSCALL_int80(getpid, 0);

	/* send start mark before stop */
	sock_send(TARGET_START_MARK, TARGET_START_MARK_SZ);
	/* send pid */
	sock_send(&self_pid, sizeof(self_pid));

	/* send original ori_pid, ori_tid and stack_base */
	sock_send(&ckpt_head.pid, sizeof(ckpt_head.pid));
	sock_send(&ckpt_head.tid, sizeof(ckpt_head.tid));
	sock_send(&ckpt_head.tnr, sizeof(ckpt_head.tnr));
	sock_send(&stack_base, sizeof(stack_base));

	
#if 0
	/* don't use sigstop, use loop can simplify gdb */
	INTERNAL_SYSCALL_int80(kill, 2, self_pid, SIGSTOP);
#endif

	while(1) {
		struct timespec {
			long       ts_sec;
			long       ts_nsec;
		};
		struct timespec tm = {1, 0};
		INTERNAL_SYSCALL_int80(nanosleep, 2, &tm, NULL);
	}

	FATAL(REPLAYER_TARGET, "we shouldn't get here! we need gdb attach!!\n");

}

__attribute__((used, unused, visibility("hidden"))) void
replayer_main(volatile struct pusha_regs pusha_regs)
{
	struct thread_private_data * tpd = get_tpd();

	void ** args = tpd->old_stack_top;
	const char * interp_fn = args[0];
	const char * exec_fn = args[1];
	const char * ckpt_fn = args[2];

	VERBOSE(REPLAYER_TARGET, "interp_fn: %s\n", interp_fn);
	VERBOSE(REPLAYER_TARGET, "exec_fn: %s\n", exec_fn);
	VERBOSE(REPLAYER_TARGET, "ckpt_fn: %s\n", ckpt_fn);

	/* restore brk */
	/* it will expand the heap and maps the pages */
	int err = INTERNAL_SYSCALL_int80(brk, 1, ckpt_head.brk);
	assert((uintptr_t)err == ckpt_head.brk);

	/* load each memory region */
	struct mem_region region;
	do {

		read_from_file(ckpt_fd, &region, sizeof(region));
		if (region.start != MEM_REGIONS_END_MARK) {
			do_restore_mem_region(&region, interp_fn, exec_fn);
		}
	} while (region.start != MEM_REGIONS_END_MARK);

	INTERNAL_SYSCALL_int80(close, 1, ckpt_fd);

	/* restoring thread area */
	/* thread areas */
	TRACE(REPLAYER_TARGET, "restoring thread area\n");
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

	TRACE(REPLAYER_TARGET, "registers:\n");
	TRACE(REPLAYER_TARGET, "\teax=0x%x\n", pregs->eax);
	TRACE(REPLAYER_TARGET, "\tebx=0x%x\n", pregs->ebx);
	TRACE(REPLAYER_TARGET, "\tecx=0x%x\n", pregs->ecx);
	TRACE(REPLAYER_TARGET, "\tedx=0x%x\n", pregs->edx);
	TRACE(REPLAYER_TARGET, "\tesp=0x%x\n", pregs->esp);
	TRACE(REPLAYER_TARGET, "\tebp=0x%x\n", pregs->ebp);


	/* prepare replay: */
	/* setup offset */
	tpd->target = eip;
	TRACE(REPLAYER_TARGET, "target eip = %p\n", eip);
	TRACE(REPLAYER_TARGET, "pusha_regs at %p\n", &pusha_regs);

	/* setup function pointers */
	/* FIXME which function? */

	/* registers have been set */
	wait_for_attach();
}

// vim:ts=4:sw=4

