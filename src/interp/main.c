/* 
 * main program of a interp
 */


#define __LOADER_MAIN_C
#include <config.h>

#include <common/defs.h>
#include <common/debug.h>
#include <common/sys/personality.h>

#include <xasm/processor.h>
#include <xasm/debug.h>
#include <xasm/syscall.h>
#include <xasm/vsprintf.h>
#include <xasm/utils.h>
#include <xasm/tls.h>
#include <xasm/compiler.h>
#include <interp/auxv.h>
#include <interp/mm.h>
#include <interp/code_cache.h>
#include <interp/logger.h>

/* reexec reset the personality bit ADDR_NO_RANDOMIZE to make sure
 * the process' memory layout is idential */
static void
reexec(void * old_esp)
{
	TRACE(LOADER, "old_esp = %p\n", old_esp);
	int err = INTERNAL_SYSCALL_int80(personality, 1, 0xffffffff);
	assert(err >= 0);
	TRACE(LOADER, "personality=0x%x\n", err);

	if (err & ADDR_NO_RANDOMIZE) {
		TRACE(LOADER, "ADDR_NO_RANDOMIZE bit has already been set\n");
		return;
	}

	int persona = err | ADDR_NO_RANDOMIZE;
	TRACE(LOADER, "persona should be 0x%x\n", persona);
	err = INTERNAL_SYSCALL_int80(personality, 1, persona);
	assert(err >= 0);

	/* buildup env and cmdline */
	/* iterate over args */
	const char ** new_argv = NULL;
	const char ** new_env = NULL;
	uintptr_t * ptr = old_esp;

	int argc = *(ptr ++);
	TRACE(LOADER, "argc = %d\n", argc);
	new_argv = (const char **)(ptr);
	new_env = new_argv + argc + 1;

	/* execve */
	err = INTERNAL_SYSCALL_int80(execve, 3, new_argv[0], new_argv, new_env);
	FATAL(LOADER, "execve failed: %d\n", err);
	return;
}

extern void * loader(void * oldesp, int * pesp_add);
__attribute__((used, unused, visibility("hidden"))) int
xmain(volatile struct pusha_regs regs)
{
	relocate_interp();
	void * oldesp = (void*)stack_top(&regs) + sizeof(uintptr_t);
	VERBOSE(LOADER, "oldesp=%p\n", oldesp);
	reexec(oldesp);
	find_auxv(oldesp);

	int esp_add = 0;
	void * retaddr = loader(oldesp, &esp_add);

	stack_top(&regs) += esp_add * sizeof(uintptr_t);
	void ** pretaddr = (void**)(stack_top(&regs));

	/* build first thread local area */
	init_tls();
	DEBUG(LOADER, "pid from tpd: %d; tid from tpd: %d\n",
			get_tpd()->pid, get_tpd()->tid);

	/* for debug use */
	print_auxv();

	*pretaddr = retaddr;
	return esp_add;
}

#undef __LOADER_MAIN_C

// vim:ts=4:sw=4

