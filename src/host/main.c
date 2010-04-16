/* 
 * main.c
 * by WN @ Apr. 10, 2010
 *
 * gdb loader's main program
 */


#include <common/defs.h>
#include <common/debug.h>
#include <interp/checkpoint.h>
#include <host/snitchaser_args.h>
#include <sys/stat.h>
#include <sys/ptrace.h>

static void
do_recover(struct opts * opts)
{
	/* execve target */
	/* check the position of stack */
	/* how about vdso? */
	/* map the tls stack */
}

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

int
main(int argc, char * argv[])
{
	struct opts * opts = parse_args(argc, argv);
	/* check opts */
	TRACE(LOADER, "target exec: %s\n", opts->target_fn);
	TRACE(LOADER, "target checkpoint: %s\n", opts->ckpt_fn);
	TRACE(LOADER, "pthread_so_fn: %s\n", opts->pthread_so_fn);
	TRACE(LOADER, "fix_pthread_tid: %d\n", opts->fix_pthread_tid);

	if (!file_exist(opts->target_fn))
		FATAL(LOADER, "target file %s doesn't exist\n", opts->target_fn);
	if (!file_exist(opts->ckpt_fn))
		FATAL(LOADER, "checkpoint file %s doesn't exist\n", opts->ckpt_fn);

	do_recover(opts);

	return 0;
}

// vim:ts=4:sw=4

