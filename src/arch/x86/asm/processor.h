#ifndef __X86_PROCESSOR_H
#define __X86_PROCESSOR_H

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

#endif

// vim:ts=4:sw=4

