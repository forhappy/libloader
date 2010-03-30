/* 
 * loader.c
 * by WN @ Feb. 08, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <common/assert.h>

#include <xasm/debug.h>
#include <xasm/syscall.h>
#include <xasm/vsprintf.h>
#include <xasm/utils.h>
#include <xasm/elf.h>

#include <interp/auxv.h>

#include <sys/mman.h>

/* 'INTERP_FILE' has already defined in config.h */
/* #define INTERP_FILE		"/lib/ld-linux.so.2" */

static void
usage(const char ** argv)
{
	__printf("Usage: %s EXECUTABLE-FILE [ARGS-FOR-PROGRAM...]\n",
			argv[0]);
	__exit(0);
}

static int
load_real_exec(void * esp)
{
	int * pargc = (int*)(esp);
	const char ** argv = &((const char**)(esp))[1];
	assert(*pargc > 0);
	if (*pargc == 1) {
		usage(argv);
	}

	*auxv_info.p_user_entry =
		load_elf(argv[1], NULL, auxv_info.ppuser_phdrs,
				auxv_info.p_nr_user_phdrs);
	*auxv_info.p_execfn = argv[1];
	pargc[1] = pargc[0] - 1;
	return 1;
}

#ifndef _START_SYMBOL
# define _START_SYMBOL _start
#endif

/* hidden symbol will be retrived using ebx directly,
 * avoid to generate R_386_GLOB_DAT relocation */
/* _START_SYMBOL is a MACRO defined in commandline, see CMakeLists.txt */
extern int _START_SYMBOL[] ATTR_HIDDEN;
void * loader(void * oldesp, int * pesp_add)
{
	assert(auxv_info.ppuser_phdrs != NULL);
	assert(auxv_info.p_nr_user_phdrs != NULL);
	assert(pesp_add != NULL);

	/* shall we load the target executable first? */
	int esp_add = 0;
	if (_START_SYMBOL == *(auxv_info.p_user_entry)) {
		VERBOSE(LOADER, "loader is called directly\n");
		esp_add = load_real_exec(oldesp);
	}
	*pesp_add = esp_add;

	/* shall we load the real interp? */
	for (int i = 0; i < *(auxv_info.p_nr_user_phdrs); i++) {
		if ((*(auxv_info.ppuser_phdrs))[i].p_type == PT_INTERP) {
			/* this is dynamically linked executable */
			void * interp_entry =
				load_elf(INTERP_FILE, auxv_info.p_base, NULL, NULL);
			return interp_entry;
		}
	}
	/* this is a statically linked executable */
	return *(auxv_info.p_user_entry);
}

// vim:ts=4:sw=4

