/* 
 * test_ptrace.c
 */

#include <linux/user.h>
#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"
#include "elfutils.h"


int main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		struct user_regs_struct regs;
		ptrace_execve("./target", argv);
		regs = ptrace_peekuser();
		FORCE(SYSTEM, "eip=0x%x, esp=0x%x, ebp=0x%x\n",
				regs.eip, regs.esp, regs.ebp);
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

