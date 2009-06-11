/* 
 * test_ptrace.c
 */

#include <linux/user.h>
#include <elf.h>
#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"
#include "procutils.h"
#include "utils.h"
#include "elfutils.h"


int main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		struct user_regs_struct regs;
		pid_t child_pid;
		child_pid = ptrace_execve("./target", argv);
		regs = ptrace_peekuser();
		FORCE(SYSTEM, "eip=0x%x, esp=0x%x, ebp=0x%x\n",
				regs.eip, regs.esp, regs.ebp);

		/* dump the stack */
		uintptr_t s_start, s_end;
		proc_get_range(child_pid, "[stack]", &s_start, &s_end);
		FORCE(SYSTEM, "stack range: 0x%x -- 0x%x\n", s_start, s_end);
		assert(regs.esp < s_end);

		void * stack = malloc(s_end - regs.esp);
		assert(stack != NULL);
		ptrace_dupmem(stack, regs.esp, s_end - regs.esp);

		/* process stack */
		/* argc: */
		FORCE(SYSTEM, "argc=%d\n", *(unsigned int *)stack);
		/* args */
		char ** pargs = stack + sizeof(unsigned int);
		int i = 0;
		while (*pargs != NULL) {
			FORCE(SYSTEM, "argv[%d] = 0x%x ", i, *pargs);
			FORCE_CONT(SYSTEM, "%s\n", ((unsigned long)(*pargs)
						- regs.esp) + stack);
			if (i == 0) {
				char newname[] = "qaaZZxxbbwq ";
				uint32_t saved_name = 0;

				/* test updmem */
				ptrace_updmem(newname, (uint32_t)(*pargs), 11);
				saved_name = (uint32_t)(*pargs);

				/* test dupmem */
				memset(newname, '\0', 11);
				ptrace_dupmem(newname, saved_name, 11);
				WARNING(SYSTEM, "redup newname=%s\n", newname);
			}
			i ++;
			pargs ++;
		}

		/* envs */
		char ** penv = pargs + 4;
		i = 0;
		while (*penv != NULL) {
			FORCE(SYSTEM, "env[%d] = 0x%x ", i, *penv);
			FORCE_CONT(SYSTEM, "%s\n", ((unsigned long)(*penv)
						- regs.esp) + stack);
			i ++;
			penv ++;
		}

		/* elf auxv */
		uint32_t * auxkv = (uint32_t*)(penv + 1);
		uintptr_t vdsopage = 0, vdsoentry = 0;
		i = 0;
		while (*auxkv != AT_NULL) {
			uint32_t k, v;
			k = *auxkv;
			v = *(auxkv + 1);
			FORCE(SYSTEM, "aux %d: k = 0x%x, v = 0x%x\n",
					i, k, v);
			if (k == AT_SYSINFO)
				vdsoentry = v;
			if (k == AT_SYSINFO_EHDR)
				vdsopage = v;
			auxkv += 2;
			i ++;
		}

		FORCE(SYSTEM, "vdso page = 0x%x\n", vdsopage);
		FORCE(SYSTEM, "vdso entry = 0x%x\n", vdsoentry);

		free(stack);

		/* insert a breakpoint at main */
		void * image = load_file("./target");
		struct elf_handler * handler = elf_init(image, 0);
		uintptr_t main_ptr = elf_get_symbol_address(handler, "main");
		free(image);
		FORCE(SYSTEM, "address of main func: 0x%x\n", main_ptr);

		ptrace_insert_bkpt(main_ptr);

		do {
			FORCE(SYSTEM, "wait for ckpt\n");
			ptrace_cont();
			regs = ptrace_peekuser();
			FORCE(SYSTEM, "stoppedat eip=0x%x\n", regs.eip);
		} while(regs.eip != main_ptr + 1);

		ptrace_resume();
		ptrace_cont();

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

