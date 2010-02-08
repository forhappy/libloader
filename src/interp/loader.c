/* 
 * loader.c
 * by WN @ Feb. 08, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <asm/syscall.h>

__attribute__((used, unused)) int
xmain(uintptr_t oldesp, volatile uintptr_t retcode)
{
	INTERNAL_SYSCALL_int80(exit, 1, 0);
	return 0;
}

// vim:ts=4:sw=4

