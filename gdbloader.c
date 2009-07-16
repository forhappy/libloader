
#include "debug.h"
#include "exception.h"
#include "ckptutils.h"

int
main(int argc, char * argv[])
{
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		struct ckpt_file * cf = load_ckpt_file(argv[1]);
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

