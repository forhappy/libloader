
#include "debug.h"
#include "exception.h"
#include "ckptutils.h"
#include "currf2_args.h"

#include <sys/stat.h>

static struct opts * opts = NULL;
static struct ckpt_file * cf = NULL;
static pid_t child_pid = -1;

static void
gdbloader_main(const char * target_fn)
{
	/* check: target_fn should be same as argv[0] */
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
		struct ckpt_file * cf = load_ckpt_file(opts->ckpt_fn);
		gdbloader_main(target_fn);
		close_ckpt_file(cf);
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			break;
		default:
			if (cf != NULL) {
				close_ckpt_file(cf);
				cf = NULL;
			}
			print_exception(FATAL, SYSTEM, exp);
			break;
	}
	do_cleanup();
	return 0;
}
// vim:ts=4:sw=4

