/* 
 * test_ptrace.c
 */

#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"
#include "elfutils.h"


int main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		ptrace_execve("./target", argv);
		ptrace_detach(TRUE);
	} CATCH(exp) {
		case EXCEPTION_NO_ERROR:
			VERBOSE(SYSTEM, "Finish successfully\n");
			break;
		default:
			print_exception(FATAL, SYSTEM, exp);
			break;
	}
	do_cleanup();
	return 0;
}

// vim:ts=4:sw=4

