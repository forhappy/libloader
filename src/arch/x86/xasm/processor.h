#ifndef __X86_PROCESSOR_H
#define __X86_PROCESSOR_H
#include <stdint.h>
/*  struct user_regs_struct */
#include <sys/user.h>

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

#define GDT_ENTRY_TLS_MIN	6
#define GDT_ENTRY_TLS_ENTRIES 3
#define GDT_ENTRY_TLS_MAX 	(GDT_ENTRY_TLS_MIN + GDT_ENTRY_TLS_ENTRIES - 1)

struct user_desc {
	unsigned int  entry_number;
	unsigned int  base_addr;
	unsigned int  limit;
	unsigned int  seg_32bit:1;
	unsigned int  contents:2;
	unsigned int  read_exec_only:1;
	unsigned int  limit_in_pages:1;
	unsigned int  seg_not_present:1;
	unsigned int  useable:1;
};

struct i387_fxsave_struct {
	uint16_t	cwd;
	uint16_t	swd;
	uint16_t	twd;
	uint16_t	fop;
	union {
		struct {
			uint64_t	rip;
			uint64_t	rdp;
		};
		struct {
			uint32_t	fip;
			uint32_t	fcs;
			uint32_t	foo;
			uint32_t	fos;
		};
	};
	uint32_t	mxcsr;
	uint32_t	mxcsr_mask;
	uint32_t	st_space[32];	/* 8*16 bytes for each FP-reg = 128 bytes */
	uint32_t	xmm_space[64];	/* 16*16 bytes for each XMM-reg = 256 bytes */
	uint32_t	padding[24];
};

struct reg_state {
	struct user_regs_struct user_regs;
	struct i387_fxsave_struct fx_state;
};


/* caller must ensure fx is aligned by 16 bytes */
inline static void
__save_i387(struct i387_fxsave_struct * fx)
{
	asm volatile (
		"fxsave	(%[fx])\n\t"
		: "=m" (*fx)
		: [fx] "cdaSDb" (fx)
	);
}

inline static void
__restore_i387(struct i387_fxsave_struct * fx)
{
	asm volatile (
			"fxrstor (%[fx])\n\t"
			:
			: [fx] "m" (*fx));
}

inline static void
restore_reg_state(struct reg_state * p, struct pusha_regs * r,
		void ** peip)
{
	struct user_regs_struct * u = &(p->user_regs);
	struct pad_fx {
		struct i387_fxsave_struct fx;
		uint8_t __padding[16];
	} pad_fx;
	struct i387_fxsave_struct * fx = ALIGN_UP(&(pad_fx.fx), 16);
	*fx = p->fx_state;

	r->ebx = u->ebx;
	r->ecx = u->ecx;
	r->edx = u->edx;
	r->esi = u->esi;
	r->edi = u->edi;
	r->ebp = u->ebp;
	r->eax = u->eax;

	if (peip != NULL)
		*peip = (void*)u->eip;
	r->flags = u->eflags;
	r->esp = u->esp;

#define restorsr(s, r) asm volatile("movl %%eax, %%" #r : : "a" (s))
#if 0
	restorsr(u->xcs, cs);
	restorsr(u->xds, ds);
	restorsr(u->xes, es);
	restorsr(u->xss, ss);
#endif
	restorsr(u->xfs, fs);
	restorsr(u->xgs, gs);
#undef loadsr

	__restore_i387(fx);
}

inline static void
build_reg_state(struct reg_state * p, struct pusha_regs * r,
		void * eip)
{
	struct user_regs_struct * u = &(p->user_regs);
	struct pad_fx {
		struct i387_fxsave_struct fx;
		uint8_t __padding[16];
	} pad_fx;
	struct i387_fxsave_struct * fx = ALIGN_UP(&(pad_fx.fx), 16);

	/* fill u */

	u->ebx = r->ebx;
	u->ecx = r->ecx;
	u->edx = r->edx;
	u->esi = r->esi;
	u->edi = r->edi;
	u->ebp = r->ebp;
	u->eax = r->eax;
	u->orig_eax = r->eax;
	u->eip = (uint32_t)(eip);
	u->eflags = r->flags;
	u->esp = r->esp;

#define readsr(d, r) asm volatile("movl %%" #r ", %%eax" : "=a" (d))
	readsr(u->xcs, cs);
	readsr(u->xds, ds);
	readsr(u->xes, es);
	readsr(u->xfs, fs);
	readsr(u->xgs, gs);
	readsr(u->xss, ss);
#undef readsr

	/* i387 fx */
	__save_i387(fx);

	p->fx_state = *fx;
}
#endif

// vim:ts=4:sw=4

