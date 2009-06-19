/* 
 * currf2.c
 * by WN @ Jun. 19, 2009
 */

#include <elf.h>
#include <linux/user.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"
#include "procutils.h"
#include "utils.h"
#include "elfutils.h"
#include "currf2_args.h"

#define ETHROW(x...)	assert_errno_throw(x)
#define ATHROW(c, x...) assert_throw(c, x)

static struct opts * opts = NULL;
static char * target_file = NULL;
static char * inject_so_file = NULL;

static void
check_file(char * fn)
{
	struct stat s;
	stat(fn, &s);
	ETHROW("stat file %s failed", fn);
	ATHROW(S_ISREG(s.st_mode), "file %s not regular file", fn);
	return;
}

static void
currf2_main(int argc, char * argv[])
{
	/* check files */
	check_file(target_file);
	check_file(inject_so_file);
	SYS_TRACE("target exec: %s\n",
			target_file);
	SYS_TRACE("inject so file: %s\n",
			inject_so_file);
	return;
}

int
main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	opts = parse_args(argc, argv);
	assert(opts != NULL);

	target_file = argv[opts->cmd_idx];
	inject_so_file = opts->inj_so;

	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		currf2_main(argc, argv);
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			break;
		default:
			ptrace_kill();
			print_exception(FATAL, SYSTEM, exp);
			break;
	}
	do_cleanup();
	return 0;
}


// vim:ts=4:sw=4

