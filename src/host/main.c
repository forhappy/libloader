/* 
 * main.c
 * by WN @ Apr. 10, 2010
 *
 * gdb loader's main program
 */

#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <common/replay/socketpair.h>

#include <interp/checkpoint.h>
#include <host/snitchaser_opts.h>
#include <host/ptrace.h>
#include <host/procmaps.h>
#include <host/exception.h>
#include <host/mm.h>
#include <host/elf.h>
#include <host/gdbserver/snitchaser_patch.h>
#include <host/replay_log.h>

#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* will be exported to gdbserver */
/* target process uses [1] and host use [0] */
int socket_pair_fds[2] = {-1, -1};

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
	int fd = open(fn, O_RDONLY);
	if (fd < 0)
		THROW_FATAL(EXP_PROC_MAPS, "open file %s failed", fn);

	define_exp(exp);
	TRY(exp) {
		char proc_fd_name[64];
		snprintf(proc_fd_name, 64, "/proc/self/fd/%d", fd);
		full_name = readlink_malloc(proc_fd_name);
		assert(full_name != NULL);
	} FINALLY {
		close(fd);
	} NO_CATCH(exp);
	return full_name;
}

static struct checkpoint_head *
check_header(void * ptr)
{
	struct checkpoint_head * phead = ptr;

	/* check magic */
	if (memcmp(phead->magic, CKPT_MAGIC, CKPT_MAGIC_SZ) != 0)
		THROW_FATAL(EXP_WRONG_CKPT, "checkpoint magic error");

	VERBOSE(REPLAYER_HOST, "ckpt magic ok\n");
	VERBOSE(REPLAYER_HOST, "brk=0x%x\n", phead->brk);
	VERBOSE(REPLAYER_HOST, "pid=%d\n", phead->pid);
	VERBOSE(REPLAYER_HOST, "tid=%d\n", phead->tid);

	return phead;
}

static void
check_ckpt(struct checkpoint_head * phead, void * boundry)
{
	/* check the 'ckpt end mark' */
	uint32_t * p_end_mark = boundry - sizeof(uint32_t);
	if (*p_end_mark != CKPT_END_MARK)
		THROW_FATAL(EXP_WRONG_CKPT, "checkpoint corrupted: no end mark");

	void * ckpt_regions = &phead[1];
	struct mem_region * r = ckpt_regions;
	while (r->start != MEM_REGIONS_END_MARK) {
		if ((void*)(r) >= boundry)
			THROW_FATAL(EXP_WRONG_CKPT,
					"checkpoint corrupted: out of boundry");
		if (r->end <= r->start)
			THROW_FATAL(EXP_WRONG_CKPT,
					"checkpoint corrupted: data corrupted");
		r = next_region(r);
	}
}

static struct checkpoint_head *
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
	if (sz < sizeof(struct checkpoint_head))
		THROW_FATAL(EXP_WRONG_CKPT, "ckpt corrupted: too small");
	void * ptr = mmap(NULL, sz, PROT_READ, MAP_PRIVATE, fd, 0);
	assert((int)(ptr) != -1);
	assert(ptr != NULL);
	close(fd);
	
	struct checkpoint_head * phead = check_header(ptr);
	check_ckpt(phead, ptr + sz);

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
		char *** pargs, char *** penvs,
		char ** exec_fn, char ** exec_fn_2)
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

	assert(nr_args >= 1);
	/* the second argument. if the process is started by
	 * libinterp.so, this is the real executable name */
	*exec_fn_2 = args[1];

	*pargs = args;
	*penvs = envs;
}

static void
compare_section(uintptr_t start, uintptr_t end, pid_t pid, void * ref)
{
	int sz = end - start;
	void * tmp_ptr = xmalloc(sz);
	assert(tmp_ptr != NULL);

	define_exp(exp);
	TRY(exp) {
		ptrace_dupmem(pid, tmp_ptr, start, sz);
		if (memcmp(tmp_ptr, ref, sz) != 0)
			THROW_FATAL(EXP_WRONG_CKPT, "libinterp.so inconsistent");
	} FINALLY {
		xfree_null(tmp_ptr);
	} NO_CATCH(exp);
}

static void
do_read_ckpt(struct opts * opts)
{
	struct checkpoint_head * phead = map_ckpt(opts->ckpt_fn);
	void * ckpt_regions = &phead[1];

	void * ptr = ckpt_regions;
	while (*((uint32_t*)(ptr)) != MEM_REGIONS_END_MARK) {
		struct mem_region * r = ptr;
		assert(r->end > r->start);
		printf("%08x-%08x:%08x:%01x %s\n", r->start, r->end,
				r->offset, r->prot, r->fn);
		ptr = next_region(r);
	}
}

/* 
 * this is the callback used in ptrace_execve and will be executed
 * immediately after fork() in child process.
 */
static void
dup_sockpair(void * unuse ATTR_UNUSED)
{
	assert(HOST_SOCKPAIR_FD != -1);
	assert(TARGET_SOCKPAIR_FD_TEMP != -1);

	/* close host socket because we don't need it */
	close(HOST_SOCKPAIR_FD);

	TRACE(REPLAYER_HOST, "dup2 fd %d to %d in target process\n",
			TARGET_SOCKPAIR_FD_TEMP, TARGET_SOCKPAIR_FD);

	/* dup TARGET_SOCKPAIR_FD_TEMP to TARGET_SOCKPAIR_FD */
	if (TARGET_SOCKPAIR_FD_TEMP == TARGET_SOCKPAIR_FD)
		return;
	/* use dup2 to do it */
	int err;
	err = dup2(TARGET_SOCKPAIR_FD_TEMP, TARGET_SOCKPAIR_FD);
	if (err != TARGET_SOCKPAIR_FD) {
		FATAL(REPLAYER_HOST, "dup2(%d, %d) failed with errno %d, return value %d\n",
				TARGET_SOCKPAIR_FD_TEMP, TARGET_SOCKPAIR_FD, errno, err);
	}
	/* close TARGET_SOCKPAIR_FD_TEMP because we don't need it anymore */
	close(TARGET_SOCKPAIR_FD_TEMP);
}

static pid_t
do_recover(struct opts * opts)
{
	/* load checkpoint file, extract command line and env */
	pid_t ret = -1;

	catch_var(pid_t, target_pid, -1);
	catch_var(char *, maps_data, NULL);
	catch_var(const char *, exec_full_fn, NULL);
	catch_var(const char *, exec_full_fn_2, NULL);
	catch_var(char **, args, NULL);
	catch_var(char **, envs, NULL);
	define_exp(exp);
	TRY(exp) {
		struct checkpoint_head * phead = map_ckpt(opts->ckpt_fn);

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
		/* if the process is start by libinterp.so, use exec_fn_2 */
		char * exec_fn_2 = NULL;
		build_argp(vargp_first, vargp_last, &args, &envs,
				&exec_fn, &exec_fn_2);
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

		set_catched_var(exec_full_fn, get_full_name(exec_fn));
		assert(exec_full_fn != NULL);

		/* if exec_full_fn same as interp_so_full_name, the real exec file
		 * is from exec_fn_2. */
		if (strcmp(exec_full_fn, opts->interp_so_full_name) == 0) {
			TRACE(REPLAYER_HOST, "target program %s is started by libinterp.so\n",
					exec_fn_2);
			if (!file_exist(exec_fn_2))
				THROW_FATAL(EXP_FILE_NOT_FOUND,
						"file %s doesn't exist, check your cwd and try again\n",
						exec_fn_2);

			set_catched_var(exec_full_fn_2, get_full_name(exec_fn_2));
			assert(exec_full_fn_2 != NULL);
		}

		/* don't use 'parent_execve': we have gdbserver */
		set_catched_var(target_pid,
				ptrace_execve(args, envs, exec_fn, FALSE, dup_sockpair, NULL));

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
		TRACE(REPLAYER_HOST, "libinterp is consistent\n");

		/* find the debug symbol */
		uintptr_t entry = elf_find_symbol(opts->interp_so_full_name,
				proc_interp.start, REPLAYER_ENTRY_STR);

		TRACE(REPLAYER_HOST, "debug entry at 0x%x\n", entry);

		/* goto */
		ptrace_goto(target_pid, entry);

		/* fix pthread */
		/* the arguments of interp:
		 * xmain(const char * ckpt_name, const char * pthread_fn,
		 * 			uintptr_t p__stack_user,
		 * 			uintptr_t pstack_used)
		 */

		/* adjust stack */
		/* arguments transfer to replayer:
		 * full filename of libinterp.so;
		 * full filename of executable;
		 * full filename of checkpoint;
		 * */
		uintptr_t pos_ckpt_fn = ptrace_push(target_pid,
				opts->ckpt_fn, strlen(opts->ckpt_fn) + 1);
		uintptr_t pos_exec_fn;
		if (exec_full_fn_2 != NULL) {
			pos_exec_fn = ptrace_push(target_pid,
				exec_full_fn_2, strlen(exec_full_fn_2) + 1);
		} else {
			pos_exec_fn = ptrace_push(target_pid,
				exec_full_fn, strlen(exec_full_fn) + 1);
		}
		uintptr_t pos_interp_fn = ptrace_push(target_pid,
				opts->interp_so_full_name,
				strlen(opts->interp_so_full_name) + 1);

		ptrace_push(target_pid, &pos_ckpt_fn, sizeof(pos_ckpt_fn));
		ptrace_push(target_pid, &pos_exec_fn, sizeof(pos_exec_fn));
		ptrace_push(target_pid, &pos_interp_fn, sizeof(pos_interp_fn));

		/* detach and continue. */
		ptrace_detach(target_pid);

		ret = target_pid;

	} FINALLY {
		get_catched_var(maps_data);
		get_catched_var(args);
		get_catched_var(envs);
		get_catched_var(target_pid);
		get_catched_var(exec_full_fn);
		get_catched_var(exec_full_fn_2);
		if (maps_data != NULL)
			proc_maps_free(maps_data);
		xfree_null(args);
		xfree_null(envs);
		xfree_null(exec_full_fn);
		xfree_null(exec_full_fn_2);
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


static void
start_gdbserver(struct opts * opts, pid_t child_pid)
{
	/* first, wait for child to start */
	char start_mark[TARGET_START_MARK_SZ];
	sock_recv(start_mark, TARGET_START_MARK_SZ);
	if (strcmp(start_mark, TARGET_START_MARK) != 0)
		THROW_FATAL(EXP_SOCKPAIR_UNSYNC, "start mark mismatch");

	pid_t target_pid;
	sock_recv(&target_pid, sizeof(target_pid));
	if (target_pid != child_pid)
		THROW_FATAL(EXP_SOCKPAIR_UNSYNC, "receive pid is %d, not %d",
				target_pid, child_pid);

	VERBOSE(REPLAYER_HOST, "host and target have been connected with each other\n");

	/* retrive snitchaser info */

	sock_recv(&SN_info, sizeof(SN_info));

	VERBOSE(REPLAYER_HOST, "ori_pid: %d\n", SN_info.ori_pid);
	VERBOSE(REPLAYER_HOST, "ori_tid: %d\n", SN_info.ori_tid);
	VERBOSE(REPLAYER_HOST, "ori_tnr: %d\n", SN_info.ori_tnr);
	VERBOSE(REPLAYER_HOST, "pid: %d\n", SN_info.pid);
	VERBOSE(REPLAYER_HOST, "stack_base: %p\n", SN_info.stack_base);
	VERBOSE(REPLAYER_HOST, "patch_block: %p\n", SN_info.patch_block_func);
	VERBOSE(REPLAYER_HOST, "unpatch_block: %p\n", SN_info.unpatch_block_func);
	VERBOSE(REPLAYER_HOST, "is_branch_inst: %p\n", SN_info.is_branch_inst);


	/* the child should have stopped. do gdbserver attachment */
	/* build arguments:
	 *
	 * gdbserver [--debug] [--remote-debug] COMM --attach pid
	 *
	 * at most 6 args
	 * */

	char * args[10];
	memset(args, '\0', sizeof(args));
	int n = 0;
	args[n++] = "gdbserver";
	if (opts->gdbserver_debug)
		args[n++] = "--debug";
	if (opts->gdbserver_remote_debug)
		args[n++] = "--remote-debug";
	args[n++] = (char*)opts->gdbserver_comm;
	args[n++] = "--attach";

	char pid_str[10];
	snprintf(pid_str, 10, "%d", child_pid);
	args[n++] = pid_str;

	args[n] = NULL;

	int err = gdbserver_main(n, args);
	THROW_VAL(EXP_GDBSERVER_EXIT, err, "gdbserver_main returns %d", err);
}

static void
kill_child(pid_t child_pid)
{
	int err;
	TRACE(XGDBSERVER, "kill %d before exit\n", child_pid);

	do {
		errno = 0;
		err = waitpid(child_pid, NULL, WNOHANG);
	} while (errno == EINTR);

	if (err == ECHILD) {
		TRACE(XGDBSERVER, "%d has nolonger be a child of snitchaser\n",
				child_pid);
		return;
	}

	/* ptrace kill child_pid */
	ptrace_kill(child_pid);
}

int
main(int argc, char * argv[])
{
	struct opts * opts = parse_args(argc, argv);

	CASSERT(REPLAYER_HOST, file_exist(opts->log_fn),
			"log file (-l) %s doesn't exist\n", opts->log_fn);
	if (opts->uncompress_log) {
		uncompress_log(opts->log_fn, opts->out_log_fn);
		return 0;
	}

	open_log(opts->log_fn);


	/* check opts */
	TRACE(REPLAYER_HOST, "target checkpoint: %s\n", opts->ckpt_fn);

	CASSERT(REPLAYER_HOST, file_exist(opts->ckpt_fn),
			"checkpoint file (-c) %s doesn't exist\n", opts->ckpt_fn);
	CASSERT(REPLAYER_HOST, file_exist(opts->interp_so_fn),
			"interp so file (-i) %s doesn't exist\n", opts->interp_so_fn);
	CASSERT(REPLAYER_HOST, opts->gdbserver_comm != NULL,
			"doesn't set gdbserver COMM using '-m'\n");

	catch_var(const char *, interp_so_full_name, NULL);
	define_exp(exp);

	pid_t child_pid;

	TRY(exp) {
		set_catched_var(interp_so_full_name,
				get_full_name(opts->interp_so_fn));
		opts->interp_so_full_name = interp_so_full_name;

		if (opts->read_ckpt) {
			do_read_ckpt(opts);
			break;
		}

		/* build socketpair */
		int err;
		err = socketpair(PF_LOCAL, SOCK_DGRAM, 0, socket_pair_fds);
		CASSERT(REPLAYER_HOST, err == 0, "unable to create sockerpair\n");

		child_pid = do_recover(opts);

	} FINALLY {
		get_catched_var(interp_so_full_name);

		xfree_null(interp_so_full_name);

	} CATCH(exp) {
		RETHROW(exp);
	}

	define_exp(exp2);
	TRY(exp2) {
		start_gdbserver(opts, child_pid);
	} FINALLY {
		kill_child(child_pid);
		close_log();
	} CATCH(exp2) {
		/* the quit of gdbserver may comes here, let myself exit gracefully */
		switch (exp2.type) {
			/* kill child_pid, nomatter whether we have detached */
		case EXP_GDBSERVER_EXIT:
			/* value is the argument of _exit */
			VERBOSE(REPLAYER_HOST,
					"gdbserver call exit (or return from main) with value %d\n",
					exp2.u.val);
			exit(exp2.u.val);
			break;
		case EXP_GDBSERVER__EXIT:
			VERBOSE(REPLAYER_HOST,
					"gdbserver call _exit with value 0%o\n",
					exp2.u.val);
			_exit(exp2.u.val);
		default:
			RETHROW(exp2);
		}
	}

	return 0;
}

// vim:ts=4:sw=4

