
#include "debug.h"
#include "exception.h"
#include "ckptutils.h"
#include "currf2_args.h"

static struct opts * opts = NULL;
struct ckpt_file * cf = NULL;
static pid_t child_pid = 0;

int
main(int argc, char * argv[])
{

	DEBUG_INIT(NULL);
	opts = parse_args(argc, argv);
	assert(opts != NULL);

	char * ckpt_fn = argv[opts->cmd_idx];
	char * target_fn = argv[opts->cmd_idx + 1];

//	printf("%s, %s\n", ckpt_fn, target_fn);
	/* check for ckpt file */


//	return 0;


	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		struct ckpt_file * cf = load_ckpt_file(ckpt_fn);
		close_ckpt_file(cf);
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			break;
		default:
			print_exception(FATAL, SYSTEM, exp);
			break;
	}
	do_cleanup();
	return 0;
}
// vim:ts=4:sw=4

