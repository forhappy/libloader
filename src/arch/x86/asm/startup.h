#ifndef __X86_STARTUP_H
#define __X86_STARTUP_H

#ifndef __LOADER_MAIN_C
# error startup.h can only be included in interp/loader.c
#endif

/* xmain return stack adjustment value, in dwords */
struct pusha_regs {
	uint32_t flags;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
};

#define stack_top(r)	((r)->esp)

asm (
".globl _start\n\
 _start:\n\
 	push $0\n\
	pusha\n\
	pushf\n\
	call xmain\n\
	test %eax, %eax\n\
	jne 1f\n\
	popf\n\
	popa\n\
	ret\n\
	1:\n\
	popf\n\
	popl %edi\n\
	popl %esi\n\
	popl %ebp\n\
	addl $4, %esp\n\
	popl %ebx\n\
	popl %edx\n\
	popl %ecx\n\
	popl %eax\n\
	movl -20(%esp), %esp\n\
	ret\n\
"
);

#endif
