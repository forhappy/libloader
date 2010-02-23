#ifndef __X86_STARTUP_H
#define __X86_STARTUP_H

#ifndef __LOADER_MAIN_C
# error startup.h can only be included in interp/loader.c
#endif

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
#endif
