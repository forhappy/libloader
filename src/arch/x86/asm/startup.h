#ifndef __X86_STARTUP_H
#define __X86_STARTUP_H

#ifndef __LOADER_MAIN_C
# error startup.h can only be included in interp/loader.c
#endif

/* xmain return stack adjustment value, in dwords */
asm (
".globl _start\n\
 _start:\n\
 	push $0\n\
 	push %esp\n\
	call xmain\n\
	testl %eax, %eax\n\
	je 1f\n\
	leal (%esp, %eax, 4), %edi\n\
	movl 0x4(%esp), %ebx\n\
	movl %ebx, 0x4(%edi)\n\
	movl %edi, %esp\n\
	1:\n\
	addl $4, %esp\n\
	ret\n\
"
);
#endif
