/* 
 * wrapper.c
 * by WN @ Jun. 22, 2009
 */

#include <stdint.h>
#include "injector.h"

static char help_string[] = "this is help string\n";

/* put them to bss section */
uint32_t old_ventry;
uint32_t old_vhdr;

SCOPE void
show_help(void)
{
	INTERNAL_SYSCALL_int80(write, 3, 1, help_string, sizeof(help_string));
	INTERNAL_SYSCALL_int80(exit, 0);
	INTERNAL_SYSCALL_int80(exit, 1, 0x10);
	INTERNAL_SYSCALL_int80(exit, 2, 0x10, 0x11);
	INTERNAL_SYSCALL_int80(exit, 3, 0x10, 0x11, 0x12);
	INTERNAL_SYSCALL_int80(exit, 4, 0x10, 0x11, 0x12, 0x13);
	INTERNAL_SYSCALL_int80(exit, 5, 0x10, 0x11, 0x12, 0x13, 0x14);
	INTERNAL_SYSCALL_int80(exit, 6, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15);
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

