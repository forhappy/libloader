#ifndef __REBRANCH_X86_PROCESSOR_H
#define __REBRANCH_X86_PROCESSOR_H

/* xmain return stack adjustment value, in dwords */

#ifndef __ASSEMBLY__

#include "defs.h"

#ifndef DEFINED_PUSHA_REGS
#define DEFINED_PUSHA_REGS
/* see syscall_handler.h */
struct pusha_regs {
	uint32_t eflags;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
};
#define stack_reg	esp
#endif

#define stack_top(r)	((r)->esp)

#define is_valid_page(p)	(((uintptr_t)(p) & (PAGE_SIZE - 1)) == 0)
#define is_valid_ptr(p)	((uintptr_t)(p) > (0x1000))

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

struct i387_fxsave_struct {
	u16			cwd; /* Control Word			*/
	u16			swd; /* Status Word			*/
	u16			twd; /* Tag Word			*/
	u16			fop; /* Last Instruction Opcode		*/
	union {
		struct {
			u64	rip; /* Instruction Pointer		*/
			u64	rdp; /* Data Pointer			*/
		};
		struct {
			u32	fip; /* FPU IP Offset			*/
			u32	fcs; /* FPU IP Selector			*/
			u32	foo; /* FPU Operand Offset		*/
			u32	fos; /* FPU Operand Selector		*/
		};
	};
	u32			mxcsr;		/* MXCSR Register State */
	u32			mxcsr_mask;	/* MXCSR Mask		*/

	/* 8*16 bytes for each FP-reg = 128 bytes:			*/
	u32			st_space[32];

	/* 16*16 bytes for each XMM-reg = 256 bytes:			*/
	u32			xmm_space[64];

	u32			padding[12];

	union {
		u32		padding1[12];
		u32		sw_reserved[12];
	};

} __attribute__((aligned(16)));

struct user_regs_struct {
	unsigned long	bx;
	unsigned long	cx;
	unsigned long	dx;
	unsigned long	si;
	unsigned long	di;
	unsigned long	bp;
	unsigned long	ax;
	unsigned long	ds;
	unsigned long	es;
	unsigned long	fs;
	unsigned long	gs;
	unsigned long	orig_ax;
	unsigned long	ip;
	unsigned long	cs;
	unsigned long	flags;
	unsigned long	sp;
	unsigned long	ss;
};

static inline uint64_t
random64(void)
{
	uint32_t x1;
	uint32_t x2;
	asm volatile ("rdtsc\n" : "=a" (x1), "=d" (x2));
	return (((uint64_t)x2) << 32) + x1;
}

static inline uint32_t
random32(void)
{
	uint32_t x1;
	uint32_t x2 ATTR(unused);
	asm volatile ("rdtsc\n" : "=a" (x1), "=d" (x2));
	return x1;
}

#ifndef barrier
# define barrier() __asm ("" ::: "memory")
#endif

/* they are defined differently in user code and kernel code,
 * so never export the detail! */
struct user_regs_struct;
struct i387_fxsave_struct;

/* according to r, save reg state to u */
void
save_reg_state(struct user_regs_struct * u,
		size_t size, struct pusha_regs * r,
		void * eip);

void
save_i387_state(struct i387_fxsave_struct * u, size_t size);

/* according to u, restore reg state to r */
void
restore_reg_state(struct user_regs_struct * u,
		size_t size, struct pusha_regs * r,
		void ** p_eip);

void
restore_i387_state(struct i387_fxsave_struct * u, size_t size);

#else

#define PUSH_ALL pusha; pushf
#define POP_ALL popf; popa
#define POP_ALL_WITH_ESP popf; popa; movl -20(%esp), %esp

#endif

#define MODRM_MOD(x)	(((x) & 0xc0)>>6)
#define MODRM_REG(x)	(((x) & 0x38)>>3)
#define MODRM_RM(x)		(((x) & 0x7))
#define BUILD_MODRM(__mod, __reg, __rm)	(((__mod) << 6) + ((__reg) << 3) + ((__rm)))

#endif

// vim:ts=4:sw=4

