/* 
 * loader.c
 * by WN @ Feb. 08, 2010
 */

#define __LOADER_MAIN_C

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <asm/debug.h>
#include <asm/syscall.h>
#include <asm/string.h>
#include <asm/vsprintf.h>
#include <asm/utils.h>

#include <asm/startup.h>

__attribute__((used, unused)) static int
xmain(uintptr_t oldesp, volatile uintptr_t retcode)
{
	relocate_interp();
	WARNING("1234\n");
	__exit(0);
	return 0;
}

#undef __LOADER_MAIN_C

// vim:ts=4:sw=4

