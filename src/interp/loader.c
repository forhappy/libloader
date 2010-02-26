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

#define INTERP_FILE		"/lib/ld-linux.so.2"
//#define INTERP_FILE		"/tmp/glibc/lib/ld-linux.so.2"
//#define INTERP_FILE "/home/wn/work/glibc/build/elf/ld-linux.so.2"

extern void *
load_elf(const char * fn, void ** p_load_bias,
		struct elf32_phdr ** ppuser_phdrs, int * p_nr_user_phdrs);

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

/* entries in aux vector */
static void ** p_user_entry = NULL;
static struct elf32_phdr ** ppuser_phdrs = NULL;
static int * p_nr_user_phdrs = NULL;
static void ** p_base = NULL;
static const char ** p_execfn = NULL;


static void
find_aux(void * oldesp)
{
	/* find the aux vector */
	uintptr_t * p = oldesp;
	p++;
	while (*p != 0)
		p ++;
	p ++;
	while (*p != 0)
		p ++;
	p ++;

	/* now p is pointed to aux vector */
	TRACE(LOADER, "auxv start from %p\n", p);
	while (*p != AT_NULL) {
		switch (p[0]) {
			case AT_ENTRY: {
				p_user_entry = (void*)(&p[1]);
				SILENT(LOADER, "user entry = %p\n", *p_user_entry);
				break;
			}
			case AT_BASE: {
				p_base = (void*)&p[1];
				SILENT(LOADER, "interp base = %p\n", *p_base);
				break;
			}
			case AT_PHDR: {
				ppuser_phdrs = (void*)(&p[1]);
				SILENT(LOADER, "user phdr = %p\n", *ppuser_phdrs);
				break;
			}
			case AT_PHENT: {
				assert(p[1] == sizeof(struct elf32_phdr));
				break;
			}
			case AT_PHNUM: {
				p_nr_user_phdrs = (void*)(&p[1]);
				SILENT(LOADER, "phnum = %d\n", *p_nr_user_phdrs);
				break;
			}
			case AT_EXECFN: {
				p_execfn = (const char **)(&p[1]);
				SILENT(LOADER, "AT_EXECFN=%s\n", *p_execfn);
				break;
			}
			default:
				break;
		}
		p += 2;
	}
}

static int
load_real_exec(void * esp)
{
	int * pargc = (int*)(esp);
	const char ** argv = &((const char**)(esp))[1];
	assert(*pargc > 0);
	if (*pargc == 1) {
		printf("Usage: %s EXECUTABLE-FILE [ARGS-FOR-PROGRAM...]\n",
				argv[0]);
		__exit(0);
	}

	*p_user_entry = load_elf(argv[1], NULL, ppuser_phdrs, p_nr_user_phdrs);
	*p_execfn = argv[1];
	pargc[1] = pargc[0] - 1;
	return 1;
}

/* hidden symbol will be retrived using ebx directly,
 * avoid to generate R_386_GLOB_DAT relocation */
extern int _start[] ATTR_HIDDEN;
__attribute__((used, unused)) static int
xmain(void * __esp, volatile void * __retaddr)
{
	relocate_interp();
	void * oldesp = __esp + sizeof(uintptr_t);

	VERBOSE(LOADER, "oldesp=%p\n", oldesp);

	reexec(oldesp);
	find_aux(oldesp);

	assert(ppuser_phdrs != NULL);
	assert(p_nr_user_phdrs != NULL);

	/* shell we load the target executable first? */
	int esp_add = 0;
	if (_start == *p_user_entry) {
		VERBOSE(LOADER, "loader is called directly\n");
		esp_add = load_real_exec(oldesp);
	}

	/* now the p_user_entry, ppuser_phdrs and p_nr_user_phdrs
	 * are adjusted */

	/* shell we load the real interp? */
	for (int i = 0; i < *p_nr_user_phdrs; i++) {
		if ((*ppuser_phdrs)[i].p_type == PT_INTERP) {
			/* this is dynamically linked executable */
			void * interp_entry =
				load_elf(INTERP_FILE, p_base, NULL, NULL);

			__retaddr = interp_entry;
			return esp_add;
		}
	}
	/* this is a statically linked executable */
	__retaddr = *p_user_entry;
	return esp_add;
}

#undef __LOADER_MAIN_C

// vim:ts=4:sw=4

