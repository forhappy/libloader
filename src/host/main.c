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
#include <host/snitchaser_args.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <host/ptrace.h>

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
	assert(memcmp(phead->magic, CKPT_MAGIC, CKPT_MAGIC_SZ) == 0);

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
	return NULL;
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
		args = realloc(args, sizeof(char *) * nr_args);
		args[nr_args - 1] = ptr;

		ptr += strlen(ptr) + 1;
	}

	/* expand args */
	args = realloc(args, sizeof(char *) * (nr_args + 1));
	args[nr_args] = NULL;

	/* envs */
	while (*ptr != '\0') {
		nr_envs ++;
		envs = realloc(envs, sizeof(char *) * nr_envs);
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
do_recover(struct opts * opts)
{
	/* load checkpoint file, extract command line and env */
	void * ckpt_img = map_ckpt(opts->ckpt_fn);

	struct checkpoint_head * phead = check_header(ckpt_img);
	assert(phead != NULL);

	struct mem_region * stack_region = find_region(&phead[1], "[stack]");
	struct mem_region * vdso_region = find_region(&phead[1], "[vdso]");
	assert(stack_region != NULL);
	assert(vdso_region != NULL);

	uintptr_t argp_first = phead->argp_first;
	uintptr_t argp_last = phead->argp_last;
	assert(argp_first <= stack_region->end);
	assert(argp_first >= stack_region->start);
	assert(argp_last <= stack_region->end);
	assert(argp_last >= stack_region->start);

	void * vstack_top = region_data(stack_region);
	void * vargp_first = argp_first - stack_region->start + vstack_top;
	void * vargp_last = argp_last - stack_region->start + vstack_top;

	char ** args = NULL;
	char ** envs = NULL;
	char * exec_fn = NULL;
	build_argp(vargp_first, vargp_last, &args, &envs, &exec_fn);
	assert(args != NULL);
	assert(envs != NULL);
	assert(exec_fn != NULL);

	/* execve target and ptrace */
	CASSERT(REPLAYER, file_exist(exec_fn),
			"file %s doesn't exist, check your cwd and try again\n", exec_fn);
	pid_t target_pid = ptrace_execve(args, envs, exec_fn);

	/* check the position of stack, vdso and libinterp.so */
	/* map the tls stack */
	/* find debug entry in libinterp.so */
	/* adjust stack */
	/* detach and continue. */

out:

errout:
	ptrace_kill(target_pid);

}

int
main(int argc, char * argv[])
{
	struct opts * opts = parse_args(argc, argv);
	/* check opts */
	TRACE(REPLAYER, "target checkpoint: %s\n", opts->ckpt_fn);
	TRACE(REPLAYER, "pthread_so_fn: %s\n", opts->pthread_so_fn);
	TRACE(REPLAYER, "fix_pthread_tid: %d\n", opts->fix_pthread_tid);

	CASSERT(REPLAYER, file_exist(opts->ckpt_fn),
			"checkpoint file (-c) %s doesn't exist\n", opts->ckpt_fn);

	do_recover(opts);

	return 0;
}

// vim:ts=4:sw=4

