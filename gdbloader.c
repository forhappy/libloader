
#include "debug.h"
#include "exception.h"
#include "ckptutils.h"
#include "currf2_args.h"
#include "procutils.h"
#include "ptraceutils.h"

#include <sys/stat.h>

static struct opts * opts = NULL;
static struct ckpt_file * cf = NULL;
static pid_t child_pid = -1;

static void
inject_memory(void)
{
	/* for each mem region in ckpt file */
	for (int i = 0; i < cf->nr_regions; i++) {
		struct mem_region * r = cf->regions[i];
		SYS_TRACE("range %d: 0x%x--0x%x (0x%x:0x%x): %s\n",
				i, r->start, r->end, r->prot, r->offset, r->fn);
		/* find the region already mapped */
		struct proc_entry e;
		bool_t res;
		res = proc_find_in_range(&e, child_pid, r->start, r->end);
		if (res) {
			SYS_TRACE("\talready mapped in target: 0x%x--0x%x (0x%x:0x%x): %s\n",
					e.start, e.end, e.prot, e.offset, e.fn);
		}
	}
}

static void
gdbloader_main(const char * target_fn)
{
	/* check: target_fn should be same as argv[0] */
	if (strcmp(target_fn, cf->cmdline[0]) != 0) {
		SYS_FATAL("target should be %s, not %s\n", cf->cmdline[0], target_fn);
		THROW(EXCEPTION_FATAL, "cmdline error");
	}

	/* execve child */
	child_pid = ptrace_execve(target_fn, cf->cmdline);

	/* inject memory */
	inject_memory();

	return;
}

int
main(int argc, char * argv[])
{
	char * target_fn = NULL;
	DEBUG_INIT(NULL);
	opts = parse_args(argc, argv);
	assert(opts != NULL);

	target_fn = argv[opts->cmd_idx];
	assert(opts->cmd_idx < argc);

	/* check ckpt file and target file */
	struct stat s;
	if (stat(opts->ckpt_fn, &s) != 0) {
		SYS_ERROR("file %s doesn't exists\n", opts->ckpt_fn);
		return -1;
	}

	if (!S_ISREG(s.st_mode)) {
		SYS_ERROR("file %s not a regular file\n", opts->ckpt_fn);
		return -1;
	}

	if (stat(target_fn, &s) != 0) {
		SYS_ERROR("file %s doesn't exists\n", target_fn);
		return -1;
	}

	if (!S_ISREG(s.st_mode)) {
		SYS_ERROR("file %s not a regular file\n", target_fn);
		return -1;
	}


	/* start */
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		cf = load_ckpt_file(opts->ckpt_fn);
		gdbloader_main(target_fn);
		close_ckpt_file(cf);
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			if (child_pid != -1)
				ptrace_kill();
			break;
		default:
			if (child_pid != -1)
				ptrace_kill();
			print_exception(FATAL, SYSTEM, exp);
			break;
	}
	do_cleanup();
	return 0;
}
// vim:ts=4:sw=4

