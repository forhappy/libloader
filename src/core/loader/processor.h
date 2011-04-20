#ifndef __PROCESSOR_H
#define __PROCESSOR_H

/* xmain return stack adjustment value, in dwords */

#ifndef __ASSEMBLY__

#include <defs.h>

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

