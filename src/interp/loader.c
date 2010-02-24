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

#include <linux/auxvec.h>

extern void *
load_real_interp(void);

extern void * real_interp_address;

/* reexec reset the personality bit ADDR_NO_RANDOMIZE to make sure
 * the process' memory layout is idential */
/* FIXME
 *
 * we should fix args here, which enables us to use
 * xxx.so aaa bbb ccc
 * to run aaa, like ld-linux.so.2
 * */
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

/* entries in aux vector */
static void ** p_user_entry = NULL;
static struct elf32_phdr ** ppuser_phdrs = NULL;
static int * p_nr_user_phdrs = NULL;

/* entries in base stack */
static int * p_user_argc = NULL;
static const char ** user_argv = NULL;
static const char ** user_env = NULL;

static void
fix_aux(void * oldesp)
{
	/* fix the aux vector */
	assert(real_interp_address != NULL);

	/* find the aux vector */
	uint32_t * p = oldesp;
	p_user_argc = (int*)(p++);
	user_argv = (const char **)(p);
	while (*p != 0)
		p ++;
	p ++;
	user_env = (const char **)(p);
	while (*p != 0)
		p ++;
	p ++;

	/* now p is pointed to aux vector */
	while (*p != AT_NULL) {
		switch (p[0]) {
			case AT_ENTRY: {
				p_user_entry = (void*)(&p[1]);
				break;
			}
			case AT_BASE: {
				p[1] = (uint32_t)(real_interp_address);
				break;
			}
			case AT_PHDR: {
				ppuser_phdrs = (void*)(&p[1]);
				break;
			}
			case AT_PHENT: {
				assert(p[1] == sizeof(struct elf32_phdr));
				break;
			}
			case AT_PHNUM: {
				p_nr_user_phdrs = (void*)(&p[1]);
				break;
			}
			default:
				break;
		}
		p += 2;
	}
}

/* hidden symbol will be retrived using ebx directly,
 * avoid to generate R_386_GLOB_DAT relocation */
extern int _start[] ATTR_HIDDEN;
__attribute__((used, unused)) static int
xmain(void * retptr, volatile void * __retaddr)
{
	relocate_interp();
	void * oldesp = retptr + sizeof(uintptr_t);
	
	reexec(oldesp);

	/* need change!! */
	void * interp_entry = load_real_interp();

	fix_aux(oldesp);

	VERBOSE(LOADER, "interp_entry = %p, user_entry = %p\n",
			interp_entry, *p_user_entry);
	if (_start == *p_user_entry) {
		VERBOSE(LOADER, "loader is called directly\n");
		__exit(1);
	}
	__retaddr = interp_entry;
	return 0;
}

#undef __LOADER_MAIN_C

// vim:ts=4:sw=4

