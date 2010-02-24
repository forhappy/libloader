/* 
 * loader.c
 * by WN @ Feb. 08, 2010
 */

#define __LOADER_MAIN_C

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>
#include <sys/personality.h>

#include <asm/debug.h>
#include <asm/syscall.h>
#include <asm/vsprintf.h>
#include <asm/utils.h>
#include <asm/elf.h>
#include <asm/startup.h>

extern void *
load_interp(void *);

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
	uint32_t * ptr = old_esp;

	int argc = *(ptr ++);
	TRACE(LOADER, "argc = %d\n", argc);
	new_argv = (const char **)(ptr);
	new_env = new_argv + argc + 1;

	/* execve */
	err = INTERNAL_SYSCALL_int80(execve, 3, new_argv[0], new_argv, new_env);
	FATAL(LOADER, "execve failed: %d\n", err);
	return;
}

__attribute__((used, unused)) static int
xmain(void * retptr, volatile void * retaddr)
{
	relocate_interp();
	void * oldesp = retptr + sizeof(uintptr_t);
	
	reexec(oldesp);

	/* need change!! */
	retaddr = load_interp(oldesp);

	__exit(0);
	return 0;
}

#undef __LOADER_MAIN_C

// vim:ts=4:sw=4

