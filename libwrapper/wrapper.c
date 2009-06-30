/* 
 * wrapper.c
 * by WN @ Jun. 22, 2009
 */

#include <stdint.h>
#include "injector.h"
#include "injector_utils.h"
#include "injector_debug.h"
#include "checkpoint/checkpoint.h"


static char help_string[] = "This is help string\n";

/* put them to bss section */
uint32_t old_ventry;
uint32_t old_vhdr;

SCOPE void
show_help(void)
{
	printf(help_string);
}

SCOPE void
injector_entry(uint32_t main_addr, uint32_t old_vdso_ventry, uint32_t old_vdso_vhdr)
{

	INJ_FORCE("Here! we come to injector!!!\n");
	INJ_FORCE("0x%x, 0x%x, 0x%x\n", old_vdso_vhdr, old_vdso_ventry, main_addr);
	INJ_FORCE("brk=0x%x\n", state_vector.brk);

}

// vim:ts=4:sw=4

