/* 
 * loader.c
 * by WN @ Feb. 08, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <asm/debug.h>
#include <asm/syscall.h>
#include <asm/string.h>
#include <asm/vsprintf.h>

asm (
".globl _start\n\
 _start:\n\
 	push $0\n\
 	push %esp\n\
	call xmain\n\
	addl $4, %esp\n\
	ret\n\
"
);

static char data[4096];

__attribute__((used, unused)) static int
xmain(uintptr_t oldesp, volatile uintptr_t retcode)
{
	sprintf(data, "test %d\n", 1234);
	INTERNAL_SYSCALL_int80(write, 3, 1, "test\n", 5);
	INTERNAL_SYSCALL_int80(write, 3, 1, data, strlen(data));
	INTERNAL_SYSCALL_int80(exit, 1, 0);
	return 0;
}

// vim:ts=4:sw=4

