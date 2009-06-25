/* 
 * wrapper.c
 * by WN @ Jun. 22, 2009
 */

#include <stdint.h>
#include "injector.h"
#include "utils.h"

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
injector_entry(void)
{
	asm volatile (
			"\txchgl %%ebx, %%esi\n"
			"\tint $0x80\n"
			"\txchgl %%ebx, %%esi\n"
			:
			: "a" (0x4), "S" (1), "c" (help_string), "d" (sizeof(help_string))
			);

	char str[64];
	int len;
	len = snprintf(str, 64, "XX:0x%x:XX\n", 0x1234);

	asm volatile (
			"\txchgl %%ebx, %%esi\n"
			"\tcall __vsyscall\n"
			"\txchgl %%ebx, %%esi\n"
			:
			: "a" (0x4), "S" (1), "c" (str), "d" (len)
			);


	asm volatile (
			"\tint $0x80\n"
			:
			: "a" (0x1)
			);

}

// vim:ts=4:sw=4

