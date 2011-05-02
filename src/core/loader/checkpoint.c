/* 
 * checkpoint.c
 * by HP.Fu @ Apr. 18, 2011
 */

#include <config.h>

#include <linux/sched.h>
#include <linux/fcntl.h>
#include <linux/mman.h>
#include <linux/stat.h>

#include <defs.h>
#include <debug.h>
#include <proc.h>
#include <loader/checkpoint.h>
#include <loader/rebranch_tpd.h>
#include <loader/tls.h>
#include <loader/mm.h>
#include <loader/startup_stack.h>
#include <loader/arch_checkpoint.h>
#include <loader/bigbuffer.h>

#ifdef COMPRESS_CKPT
# include <loader/compression.h>
#endif

#include <xasm/syscall.h>
#include <xasm/asm_syscall.h>
#include <alloca.h>
#include <interp/rebranch_asm_offset.h>

#include <xasm/stat64_fix.h>

static size_t
get_aval_size(const char * fn, uint64_t offset, size_t desire_size)
{
	desire_size = ALIGN_UP(desire_size, PAGE_SIZE);
	if ((fn == NULL) || (fn[0] == '\0'))
		return desire_size;
	if (fn[0] == '[')
		return desire_size;

	/* stat the file */
	struct stat64 st;
	int err = sys_stat64(fn, &st);
	if (err != 0) {
		WARNING(CKPT, "stat %s failed: %d\n", fn, err);
		return desire_size;
	}

	/* check size */
	size_t retval = desire_size;
	if (offset + desire_size > (uint64_t)(st.st_size))
		retval = ALIGN_UP(st.st_size - offset, PAGE_SIZE);
	return desire_size;
}

static void
writeto(int fd, const void * buffer, size_t sz)
{
	assert(sz >= 0);
	if (sz == 0)
		return;
	assert(buffer != NULL);

#ifdef COMPRESS_CKPT
	append_self_bigbuffer(fd, buffer, sz);
#else
	int err = sys_write(fd, buffer, sz);
	if (err != sz)
		FATAL(CKPT, "write to ckpt file failed: %d\n", err);
#endif
	return;
}

struct sg {
	bool_t have_imark;
	uint32_t imark;
	const void * ptr;
	size_t length;
};


static void
setup_sglist(struct sg * psg, bool_t haveim, uint32_t im,
		const void * ptr, size_t len)
{
	assert(psg != NULL);
	psg->have_imark = haveim;
	psg->imark = im;
	psg->ptr = ptr;
	psg->length = len;
}

static void
write_section_sg(int fd, enum ckpt_mark mark, struct sg * sglist, int nr)
{
	size_t sz = 0;
	assert(sglist != NULL);
	for (int i = 0; i < nr; i++) {
		if (sglist[i].have_imark) {
			sz += (sizeof(sglist[i].imark)) + (sizeof(size_t));
		}
		sz += sglist[i].length;
	}

	writeto(fd, &mark, sizeof(mark));
	writeto(fd, &sz, sizeof(sz));

	for (int i = 0; i < nr; i++) {
		if (sglist[i].have_imark) {
			writeto(fd, &sglist[i].imark, sizeof(sglist[i].imark));
			writeto(fd, &sglist[i].length, sizeof(sglist[i].length));
		}
		writeto(fd, sglist[i].ptr, sglist[i].length);
	}
}

static void
write_section(int fd, enum ckpt_mark mark, const void * ptr, size_t length)
{
	struct sg sg;
	sg.have_imark = FALSE;
	sg.ptr = ptr;
	sg.length = length;
	write_section_sg(fd, mark, &sg, 1);
}

static void
clear_all_thread_mem(void)
{
	struct tls_desc * td;
	list_for_each_entry(td, &tls_list, list) {
		free_aux_mem(td->start_addr + OFFSET_TPD);
	}
}

static void
ckpt_baseinfo(int fd)
{
	TRACE(CKPT, "writing base info\n");

	/* first, the execuable path (maybe the interp itself) */
	assert(STACK_INFO(bottom_exec_fn) != NULL);
	size_t sz = strlen(STACK_INFO(bottom_exec_fn)) + 1;
	TRACE(CKPT, "exec file: %s\n", STACK_INFO(bottom_exec_fn));

	write_section(fd, CKPT_SECT_EXEC_MARK, STACK_INFO(bottom_exec_fn), sz);

	/* then, the args */
	assert(STACK_INFO(cmdline_argbegin) != NULL);
	assert(STACK_INFO(cmdline_argend) != NULL);
	assert(STACK_INFO(cmdline_argend) > STACK_INFO(cmdline_argbegin));
	write_section(fd, CKPT_SECT_ARGS_MARK,
			STACK_INFO(cmdline_argbegin),
			STACK_INFO(cmdline_argend) - STACK_INFO(cmdline_argbegin));

	/* then, envs */
	assert(STACK_INFO(envbegin) != NULL);
	assert(STACK_INFO(envend) != NULL);
	assert(STACK_INFO(envend) > STACK_INFO(envbegin));
	write_section(fd, CKPT_SECT_ENVS_MARK,
			STACK_INFO(envbegin),
			STACK_INFO(envend) - STACK_INFO(envbegin));
}

static void
ckpt_archinfo(int fd, struct pusha_regs * regs, void * pc)
{
	uint8_t buffer[1024];
	TRACE(CKPT, "ckpt cpuinfo\n");
	size_t sz = get_ckpt_cpuinfo(buffer, 1024, regs, pc);
	TRACE(CKPT, "ckpt cpuinfo: size %ld\n", (long int)sz);
	write_section(fd, CKPT_SECT_CPU_MARK, buffer, sz);

	sz = get_tls_info(buffer, 1024);
	TRACE(CKPT, "ckpt tlsinfo: size %ld\n", (long int)sz);
	write_section(fd, CKPT_SECT_TLS_MARK, buffer, sz);
}

static void
ckpt_memseg(int fd, struct proc_mem_region r)
{
	assert(r.valid);
	char * fn_buffer = alloca(r.fn_len + 1);
	memset(fn_buffer, '\0', r.fn_len + 1);
	if (r.fn_start != NULL)
		memcpy(fn_buffer, r.fn_start, r.fn_len);
	TRACE(CKPT, "ckpting |%s|\n", fn_buffer);

	/* format of memset:
	 *
	 * [mark] [length] [fnlen(without '\0') or '0'] [fn] [size] [data]
	 * */

	/* filename, fileinfo, filedata */
	struct sg list[3];
	setup_sglist(&list[0], TRUE, CKPT_SUBSECT_FN_MARK,
			fn_buffer, r.fn_len + 1);

	struct map_file_info finfo;
	finfo.prot = r.prot;
	finfo.offset = r.offset;
	finfo.space_sz = r.end - r.start;
	finfo.ckpt_sz = 0;
	finfo.start = r.start;
	finfo.end = r.end;

	setup_sglist(&list[1], TRUE, CKPT_SUBSECT_FINFO_MARK,
			&finfo, sizeof(finfo));
	/* for special file */
	if ((r.fn_start != NULL) && (memcmp(r.fn_start, "/dev", 4) == 0)) {
		TRACE(CKPT, "this is dev file, don't ckpt it\n");
		setup_sglist(&list[2], TRUE, CKPT_SUBSECT_FDATA_MARK,
				NULL, 0);
		/* ckpt then exit */
		write_section_sg(fd, CKPT_SECT_MEMSEG_MARK,
				list, 3);
		return;
	}

	/* make target space readable */
	if (!(r.prot & PROT_READ)) {
		size_t len = r.end - r.start;
		TRACE(CKPT, "unprotect %lx to %lx\n", r.start, r.end);
		int err = sys_mprotect((void*)r.start, len, r.prot | PROT_READ);
		assert(err == 0);
	}

	/* for internal mapping */
	if ((r.fn_start == NULL) || (r.fn_start[0] == '[')) {
		TRACE(CKPT, "%lx to %lx is internal mapping\n", r.start, r.end);
		finfo.ckpt_sz = finfo.space_sz;
		setup_sglist(&list[2], TRUE, CKPT_SUBSECT_FDATA_MARK,
				(void*)(r.start), (size_t)(r.end - r.start));
	} else {
		size_t aval_sz = get_aval_size(fn_buffer, finfo.offset, finfo.space_sz);
		assert(aval_sz % PAGE_SIZE == 0);
		finfo.ckpt_sz = aval_sz;

		TRACE(CKPT, "%lx to %lx is file mapping, aval sz is %ld\n",
				r.start, r.end, (long int)(aval_sz));
		setup_sglist(&list[2], TRUE, CKPT_SUBSECT_FDATA_MARK,
				(void*)(r.start), aval_sz);
	}


	write_section_sg(fd, CKPT_SECT_MEMSEG_MARK,
			list, 3);

	if (!(r.prot & PROT_READ)) {
		size_t len = r.end - r.start;
		TRACE(CKPT, "reprotect %lx to %lx\n", r.start, r.end);
		int err = sys_mprotect((void*)r.start, len, r.prot);
		assert(err == 0);
	}
}

static void
ckpt_meminfo(int fd)
{
	/* we alloc 3M cont pages, then unmap 1st 1M and last 1M to ensure
	 * the procmap pages is disjoint with other mempry sections */
	void * g1 = xalloc_pages(MAX_PROC_MAPS_FILE_SZ +
			2 * PROCMAP_GUARD, FALSE);
	assert(g1 != NULL);
	void * procmap_ptr = g1 + PROCMAP_GUARD;
	void * g2 = procmap_ptr + MAX_PROC_MAPS_FILE_SZ;

	xdealloc_pages(g1, PROCMAP_GUARD);
	xdealloc_pages(g2, PROCMAP_GUARD);

	memset(procmap_ptr, '\0', MAX_PROC_MAPS_FILE_SZ);

	/* read proc file */
	struct proc_mem_handler h;
	read_self_procmap(&h, procmap_ptr);
	TRACE(CKPT, "proc map from %p to %p\n", h.map_data, h.map_end);

	struct proc_mem_region r = read_maps_line(&h);
	while (r.valid) {
		if (r.start == (uintptr_t)procmap_ptr) {
			r = read_map_line(&h);
			continue;
		}
		TRACE(CKPT, "checkpointing 0x%lx-0x%lx\n", r.start, r.end);
		ckpt_memseg(fd, r);
		r = read_map_line(&h);
	}
	xdealloc_pages(procmap_ptr, MAX_PROC_MAPS_FILE_SZ);

	enum ckpt_mark end_mark = CKPT_SECT_END_MARK;
	size_t tmp = 0;
	writeto(fd, &end_mark, sizeof(end_mark));
	writeto(fd, &tmp, sizeof(tmp));
}

static void
do_checkpoint(struct pusha_regs * regs, void * pc, void * stack_top,
		char * fn, pid_t pid, pid_t tid)
{
	DEBUG(CKPT, "%d:%d do checkpoint\n", pid, tid);

	init_self_bigbuffer();

	/* open the file */
	int fd = sys_open(fn, O_WRONLY|O_CREAT|O_TRUNC, 0664);
	CASSERT(fd >= 0, CKPT, "open ckpt file %s failed: %d\n", fn, fd);

	TRACE(CKPT, "ckptfile: %d\n", fd);
#ifdef COMPRESS_CKPT
	write_comp_file_head(fd);
#endif

	struct checkpoint_head head;
	memset(&head, '\0', sizeof(head));
	strcpy(head.magic, CKPT_VERSION_STRING);
	head.pid = pid;
	head.tid = tid;
	head.brk = sys_brk(0);

	struct tls_desc * td = get_self_tls_desc();
	assert(td != NULL);
	head.tls_start = (uintptr_t)(td->start_addr);
	head.tls_size = td->size;
#if defined ARCH_X86_32
	head.tnr = td->nr;
#endif
	head.stack_top = (uintptr_t)(stack_top);

	writeto(fd, &head, sizeof(head));
	TRACE(CKPT, "ckpt head is written\n");

	/* code cache and mm should have been destoried */

	/* we needn't clear logger and LZO anymore. Now we use bigbuffer */
	ckpt_baseinfo(fd);
	ckpt_archinfo(fd, regs, pc);
	ckpt_meminfo(fd);

	flush_self_bigbuffer(fd);
	/* close the file */
	sys_close(fd);

	DEBUG(CKPT, "ckpt finished\n");
}

void
fork_checkpoint(struct pusha_regs * regs, void * pc,
		void * stack_top, struct timeval * ptv)
{
	assert(ptv != NULL);

	struct thread_private_data * tpd = get_self_tpd();
	pid_t pid = tpd->pid;
	pid_t tid = tpd->tid;

	TRACE(CKPT, "%d:%d fork_checkpoint\n", pid, tid);
	/* fork and wait */
	pid_t tmppid = SYSCALL_MACRO(clone, 5,
			0, 0, NULL, NULL, NULL);
	assert(tmppid >= 0);
	if (tmppid == 0) {
		tmppid = SYSCALL_MACRO(clone, 5,
				0, 0, NULL, NULL, NULL);
		assert(tmppid >= 0);
		if (tmppid == 0) {

			/* clear all thread's cc and mm */
			/* clear code cache and mm */
			clear_all_thread_mem();

			char * fn = alloca(MAX_OUTPUT_FN_LEN);
			assert(fn != NULL);
			size_t len = snprintf(fn, MAX_OUTPUT_FN_LEN - 1, "%s/%d-%d-%010u-%010u."
#ifdef COMPRESS_CKPT
					"lzockpt"
#else
					"ckpt"
#endif
					,
					LOG_DIR, pid, tid,
					(uint32_t)ptv->tv_sec, (uint32_t)ptv->tv_usec);
			assert(len < MAX_OUTPUT_FN_LEN);
			TRACE(CKPT, "ckpt name set to %s\n", fn);
			do_checkpoint(regs, pc, stack_top, fn, pid, tid);
			sys_exit(0);
		}
		sys_exit(0);
	} 
	/* wait4 */
	TRACE(CKPT, "tmp pid: %d\n", pid);
	int err = sys_wait4(tmppid, NULL, __WCLONE | __WNOTHREAD, NULL);
	assert(err == tmppid);
	TRACE(CKPT, "tmp process end\n");
}

/* fork and unmap all thread's codecache and mm */
void
dead_checkpoint(struct pusha_regs * regs, void * pc, void * stack_top)
{
	assert(regs != NULL);

	struct timeval tv;
	int err = sys_gettimeofday(&tv, NULL);
	assert(err == 0);

	struct thread_private_data * tpd = get_self_tpd();
	pid_t pid = tpd->pid;
	pid_t tid = tpd->tid;

	/* don't fork: avoid sigchld */
	pid_t chld = SYSCALL_MACRO(clone, 5,
			0, 0, NULL, NULL, NULL);
	assert(chld >= 0);
	if (chld == 0) {
		/* this is child */
		/* for each thread, clear its codecache and mmobj  */
		/* we needn't lock! */

		clear_all_thread_mem();

		char * fn = alloca(MAX_OUTPUT_FN_LEN);
		assert(fn != NULL);
		snprintf(fn, MAX_OUTPUT_FN_LEN - 1, "%s/%d-%d-%010u-%010u.dead",
				LOG_DIR, pid, tid,
				(uint32_t)tv.tv_sec, (uint32_t)tv.tv_usec);
		do_checkpoint(regs, pc, stack_top, fn, pid, tid);

		sys_exit(0);

	} else {
		/* this is parent */
		/* wait for child */
		err = sys_wait4(chld, NULL, __WCLONE | __WNOTHREAD, NULL);
		assert(err == chld);
		return;
	}

}

// vim:ts=4:sw=4

