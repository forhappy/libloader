/* 
 * main.c
 * by WN @ Apr. 10, 2010
 *
 * gdb loader's main program
 */

#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>

#include <interp/checkpoint.h>
#include <host/snitchaser_opts.h>
#include <host/ptrace.h>
#include <host/procmaps.h>
#include <host/exception.h>
#include <host/mm.h>
#include <host/elf.h>

#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static bool_t
file_exist(const char * fn)
{
	if (fn == NULL)
		return FALSE;
	struct stat st;
	int err = stat(fn, &st);
	if (err != 0)
		return FALSE;
	if (!S_ISREG(st.st_mode))
		return FALSE;
	return TRUE;
}

static void *
map_ckpt(const char * filename)
{
	assert(filename != NULL);
	int fd = open(filename, O_RDONLY);
	assert(fd > 0);

	/* the length of ckpt */
	struct stat st;
	int err = fstat(fd, &st);
	assert(err == 0);

	size_t sz = st.st_size;
	void * ptr = mmap(NULL, sz, PROT_READ, MAP_PRIVATE, fd, 0);
	assert((int)(ptr) != -1);
	assert(ptr != NULL);
	close(fd);
	return ptr;
}

static struct checkpoint_head *
check_header(void * ptr)
{
	struct checkpoint_head * phead = ptr;

	/* check magic */
	if (memcmp(phead->magic, CKPT_MAGIC, CKPT_MAGIC_SZ) != 0)
		THROW_FATAL(EXP_WRONG_CKPT, "checkpoint magic error");

	VERBOSE(REPLAYER, "ckpt magic ok\n");
	VERBOSE(REPLAYER, "brk=0x%x\n", phead->brk);
	VERBOSE(REPLAYER, "pid=%d\n", phead->pid);
	VERBOSE(REPLAYER, "tid=%d\n", phead->tid);

	return phead;
}

static struct mem_region *
find_region(void * ptr, const char * fn)
{
	while (*((uint32_t*)(ptr)) != MEM_REGIONS_END_MARK) {
		struct mem_region * r = ptr;
		assert(r->end > r->start);
		if (strcmp(r->fn, fn) == 0)
			return r;
		ptr = next_region(r);
	}
	THROW(EXP_CKPT_REGION_NOT_FOUND, "checkpoint error: miss region \"%s\"", fn);
}

static void
build_argp(void * vargp_start, void * vargp_last,
		char *** pargs, char *** penvs, char ** exec_fn)
{
	char ** args = NULL;
	char ** envs = NULL;
	int nr_args = 0;
	int nr_envs = 0;

	/* see do_execve in fs/exec.c */

	/* args */
	char * ptr = vargp_start;
	while ((void*)ptr <= vargp_last) {
		nr_args ++;
		args = xrealloc(args, sizeof(char *) * nr_args);
		args[nr_args - 1] = ptr;

		ptr += strlen(ptr) + 1;
	}

	/* expand args */
	args = xrealloc(args, sizeof(char *) * (nr_args + 1));
	args[nr_args] = NULL;

	/* envs */
	while (*ptr != '\0') {
		nr_envs ++;
		envs = xrealloc(envs, sizeof(char *) * nr_envs);
		assert(envs != NULL);
		envs[nr_envs - 1] = ptr;

		ptr += strlen(ptr) + 1;
	}

	/* the last slot of 'envs' is actually 'exec_fn' */
	*exec_fn = envs[nr_envs - 1];
	envs[nr_envs - 1] = NULL;

	*pargs = args;
	*penvs = envs;
}

static void
compare_section(uintptr_t start, uintptr_t end, pid_t pid, void * ref)
{
	catch_var(void *, tmp_ptr, NULL);
	define_exp(exp);
	TRY(exp) {
		int sz = end - start;
		set_catched_var(tmp_ptr, xmalloc(sz));
		assert(tmp_ptr != NULL);
		ptrace_dupmem(pid, tmp_ptr, start, sz);
		if (memcmp(tmp_ptr, ref, sz) != 0)
			THROW_FATAL(EXP_WRONG_CKPT, "libinterp.so inconsistent");
	} FINALLY {
		get_catched_var(tmp_ptr);
		xfree_null(tmp_ptr);
	} NO_CATCH(exp);
}

static void
fix_pthread(const char * pthread_fn, void * regions,
		uintptr_t * pp__stack_user, uintptr_t * ppstack_used)
{
	assert(pthread_fn != NULL);
	assert(regions != NULL);
	assert(pp__stack_user != NULL);
	assert(ppstack_used != NULL);
	define_exp(exp);
	TRY(exp) {
		struct mem_region * region = find_region(regions, pthread_fn);
		assert(region != NULL);

		uintptr_t load_bias = region->start;
		uintptr_t p__stack_user = elf_find_symbol(pthread_fn,
				load_bias, "__stack_user");
		uintptr_t pstack_used = elf_find_symbol(pthread_fn,
				load_bias, "stack_used");
		*pp__stack_user = p__stack_user;
		*ppstack_used = pstack_used;
	} FINALLY {

	} CATCH(exp) {
		switch (exp.type) {
		case EXP_CKPT_REGION_NOT_FOUND:
			VERBOSE(REPLAYER,
					"there's no libpthread in ckpt, things are simpler\n");
			break;
		default:
			RETHROW(exp);
		}
	}
}

static void
do_read_ckpt(struct opts * opts)
{
	void * ckpt_img = map_ckpt(opts->ckpt_fn);
	struct checkpoint_head * phead = check_header(ckpt_img);
	void * ckpt_regions = &phead[1];

	void * ptr = ckpt_regions;
	while (*((uint32_t*)(ptr)) != MEM_REGIONS_END_MARK) {
		struct mem_region * r = ptr;
		assert(r->end > r->start);
		printf("%08x-%08x: %s\n", r->start, r->end, r->fn);
		ptr = next_region(r);
	}
}

static pid_t
do_recover(struct opts * opts)
{
	/* load checkpoint file, extract command line and env */
	pid_t ret = -1;

	catch_var(pid_t, target_pid, -1);
	catch_var(char *, maps_data, NULL);
	catch_var(char **, args, NULL);
	catch_var(char **, envs, NULL);
	define_exp(exp);
	TRY(exp) {
		void * ckpt_img = map_ckpt(opts->ckpt_fn);
		struct checkpoint_head * phead = check_header(ckpt_img);

		void * ckpt_regions = &phead[1];

		struct mem_region * stack_region = find_region(ckpt_regions,
				"[stack]");
		struct mem_region * vdso_region = find_region(ckpt_regions,
				"[vdso]");
		struct mem_region * interp_region = find_region(ckpt_regions,
				opts->interp_so_full_name);

		/* extract cmd line and env */
		uintptr_t argp_first = phead->argp_first;
		uintptr_t argp_last = phead->argp_last;

		if ((argp_first > stack_region->end) ||
				(argp_first < stack_region->start))
			THROW_FATAL(EXP_WRONG_CKPT, "argp_first 0x%x not in stack",
					argp_first);
		if ((argp_last > stack_region->end) ||
				(argp_last < stack_region->start))
			THROW_FATAL(EXP_WRONG_CKPT, "argp_last 0x%x not in stack",
					argp_last);

		void * vstack_top = region_data(stack_region);
		void * vargp_first = argp_first - stack_region->start + vstack_top;
		void * vargp_last = argp_last - stack_region->start + vstack_top;

		char * exec_fn = NULL;
		build_argp(vargp_first, vargp_last, &args, &envs, &exec_fn);
		set_catched_var(args, args);
		set_catched_var(envs, envs);
		assert(args != NULL);
		assert(envs != NULL);
		assert(exec_fn != NULL);

		/* execve target and ptrace */
		if (!file_exist(exec_fn))
			THROW_FATAL(EXP_FILE_NOT_FOUND,
					"file %s doesn't exist, check your cwd and try again\n",
					exec_fn);
		set_catched_var(target_pid, ptrace_execve(args, envs, exec_fn));

		xfree_null_catched(args);
		xfree_null_catched(envs);

		/* read its '/proc/xxx/maps' */
		set_catched_var(maps_data, proc_maps_load(target_pid));

		/* find sections: [stack], [vdso] and libinterp.so */
		struct proc_mem_region proc_stack;
		struct proc_mem_region proc_vdso;
		struct proc_mem_region proc_interp;

		proc_maps_find(&proc_stack, "[stack]", maps_data);
		proc_maps_find(&proc_vdso, "[vdso]", maps_data);
		proc_maps_find(&proc_interp, opts->interp_so_full_name, maps_data);
		if ((proc_vdso.start != vdso_region->start) ||
				(proc_vdso.end != vdso_region->end))
			THROW_FATAL(EXP_WRONG_CKPT, "[vdso] section inconsistent");
		if ((proc_stack.end != stack_region->end))
			THROW_FATAL(EXP_WRONG_CKPT, "[stack] section inconsistent");

		/* compare contents of libinterp.so: only the non-writable section */
		if (proc_vdso.prot != (PROT_READ | PROT_EXEC))
			THROW_FATAL(EXP_WRONG_CKPT,
					"interp section's pretection bits inconsistent");
		compare_section(proc_interp.start, proc_interp.end,
				target_pid, region_data(interp_region));
		TRACE(REPLAYER, "libinterp is consistent\n");

		/* find the debug symbol */
		uintptr_t entry = elf_find_symbol(opts->interp_so_full_name,
				proc_interp.start, REPLAYER_ENTRY_STR);

		TRACE(REPLAYER, "debug entry at 0x%x\n", entry);

		/* goto */
		ptrace_goto(target_pid, entry);

		/* fix pthread */
		/* the arguments of interp:
		 * xmain(const char * ckpt_name, const char * pthread_fn,
		 * 			uintptr_t p__stack_user,
		 * 			uintptr_t pstack_used)
		 */
		uintptr_t p__stack_user = 0;
		uintptr_t pstack_used = 0;
		fix_pthread(opts->pthread_so_full_name, ckpt_regions,
				&p__stack_user, &pstack_used);

		/* adjust stack */
		uintptr_t pos_pthread_fn = ptrace_push(target_pid,
				opts->pthread_so_full_name,
				strlen(opts->pthread_so_full_name) + 1);
		uintptr_t pos_ckpt_fn = ptrace_push(target_pid,
				opts->ckpt_fn, strlen(opts->ckpt_fn) + 1);

		ptrace_push(target_pid, &pstack_used, sizeof(pstack_used));
		ptrace_push(target_pid, &p__stack_user, sizeof(p__stack_user));
		ptrace_push(target_pid, &pos_pthread_fn, sizeof(pos_pthread_fn));
		ptrace_push(target_pid, &pos_ckpt_fn, sizeof(pos_ckpt_fn));

		/* detach and continue. */
		ptrace_detach(target_pid);

		ret = target_pid;

	} FINALLY {
		get_catched_var(maps_data);
		get_catched_var(args);
		get_catched_var(envs);
		get_catched_var(target_pid);
		if (maps_data != NULL)
			proc_maps_free(maps_data);
		xfree_null(args);
		xfree_null(envs);

	} CATCH(exp) {
		get_catched_var(target_pid);
		if (target_pid != -1) {
			ptrace_kill(target_pid);
			set_catched_var(target_pid, -1);
		}
		RETHROW(exp);
	}
	return ret;
}


static char *
readlink_malloc(const char *filename)
{
	int size = 100;
	catch_var(char *, buffer, NULL);
	define_exp(exp);
	TRY(exp) {
		set_catched_var(buffer, xrealloc(buffer, size));
		assert(buffer != NULL);
		memset(buffer, '\0', size);
		int nchars = readlink(filename, buffer, size);
		if (nchars < 0)
			THROW_FATAL(EXP_UNCATCHABLE, "readlink failed");
		if (nchars < size)
			break;
		size *= 2;
	} FINALLY {
	} CATCH(exp) {
		get_catched_var(buffer);
		xfree_null(buffer);
		RETHROW(exp);
	}
	return buffer;
}

static const char *
get_full_name(const char * fn)
{
	const char * full_name = NULL;
	define_exp(exp);
	catch_var(int, fd, -1);
	TRY(exp) {
		set_catched_var(fd, open(fn, O_RDONLY));
		if (fd < 0)
			THROW_FATAL(EXP_PROC_MAPS, "open file %s failed", fn);
		char proc_fd_name[64];
		snprintf(proc_fd_name, 64, "/proc/self/fd/%d", fd);
		full_name = readlink_malloc(proc_fd_name);
		assert(full_name != NULL);
	} FINALLY {
		get_catched_var(fd);
		if (fd != -1)
			close(fd);
	} NO_CATCH(exp);
	return full_name;
}

int
main(int argc, char * argv[])
{
	struct opts * opts = parse_args(argc, argv);
	/* check opts */
	TRACE(REPLAYER, "target checkpoint: %s\n", opts->ckpt_fn);
	TRACE(REPLAYER, "pthread_so_fn: %s\n", opts->pthread_so_fn);

	CASSERT(REPLAYER, file_exist(opts->ckpt_fn),
			"checkpoint file (-c) %s doesn't exist\n", opts->ckpt_fn);
	CASSERT(REPLAYER, file_exist(opts->interp_so_fn),
			"interp so file (-i) %s doesn't exist\n", opts->interp_so_fn);

	catch_var(const char *, interp_so_full_name, NULL);
	catch_var(const char *, pthread_so_full_name, NULL);
	define_exp(exp);

	pid_t child_pid;

	TRY(exp) {
		set_catched_var(interp_so_full_name,
				get_full_name(opts->interp_so_fn));
		set_catched_var(pthread_so_full_name,
				get_full_name(opts->pthread_so_fn));
		opts->interp_so_full_name = interp_so_full_name;
		opts->pthread_so_full_name = pthread_so_full_name;

		if (opts->read_ckpt) {
			do_read_ckpt(opts);
			break;
		}

		child_pid = do_recover(opts);

	} FINALLY {
		get_catched_var(interp_so_full_name);
		get_catched_var(pthread_so_full_name);
		if (interp_so_full_name != NULL)
			xfree_null(interp_so_full_name);
		if (pthread_so_full_name != NULL)
			xfree_null(pthread_so_full_name);
	} CATCH(exp) {
		RETHROW(exp);
	}

	/* wait for child so that it can recieve C-c */
	/* we cannot incluse sys/wait.h because of type conflict */
	extern pid_t waitpid(pid_t, int *, int);
	waitpid(child_pid, NULL, 0);

	return 0;
}

// vim:ts=4:sw=4

