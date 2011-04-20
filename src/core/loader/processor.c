/* 
 * x86/interp/processor.c
 * by WN @ Nov. 02, 2010
 */

/*
 * modified by HP.F
 * Mar. 25, 2011
 *
 * */

#include <defs.h>
#include <debug.h>
#include <asm/user.h>
#include <linux/types.h>
#include <loader/processor.h>
#include <asm/processor.h>
#if 0 
struct user_regs_struct
get_current_regs()
{
	struct user_regs_struct reg;
#define readgr(dst, r) do { \
	asm volatile ("movl %%" #r ", %%eax": "=a"(dst));\
	(dst) & 0xffffffff;\
}while(0)
	asm("pushl %%eax");
	readgr(reg.bx, bx);
	readgr(reg.cx, cx);
	readgr(reg.dx, dx);
	readgr(reg.si, si);
	readgr(reg.di, di);
	readgr(reg.bp, bp);
	readgr(reg.ip, ip);
	readgr(reg.flags, eflags);
	asm("pop %%eax");
	readgr(reg.ax, ax);
	readgr(reg.orig_ax, ax);
	readgr(reg.sp, sp);

#define readsr(d, r) do { \
	asm volatile("movw %%" #r ", %%ax" : "=a" (d));\
	(d) &= 0xffff; \
} while (0)
	readsr(u->cs, cs);
	readsr(u->ds, ds);
	readsr(u->es, es);
	readsr(u->fs, fs);
	readsr(u->gs, gs);
	readsr(u->ss, ss);
#undef readsr
	return reg;
}
#endif

void
save_reg_state(struct user_regs_struct * u,
		size_t size, struct pusha_regs * r, void * eip)
{
	assert(u != NULL);
	assert(r != NULL);
	assert(size >= sizeof(*u));
	u->bx = r->ebx;
	u->cx = r->ecx;
	u->dx = r->edx;
	u->si = r->esi;
	u->di = r->edi;
	u->bp = r->ebp;
	u->ax = r->eax;
	u->orig_ax = r->eax;
	u->ip = (unsigned long)(eip);
	u->flags = r->eflags;
	u->sp = r->esp;

#define readsr(d, r) do { \
	asm volatile("movw %%" #r ", %%ax" : "=a" (d));\
	(d) &= 0xffff; \
} while (0)
	readsr(u->cs, cs);
	readsr(u->ds, ds);
	readsr(u->es, es);
	readsr(u->fs, fs);
	readsr(u->gs, gs);
	readsr(u->ss, ss);
#undef readsr
}

void
save_i387_state(struct i387_fxsave_struct * u, size_t size)
{
	assert(u != NULL);
	assert(size >= sizeof(*u));

	struct {
		struct i387_fxsave_struct fx;
		uint8_t __padding[16];
	} pad_fx;

	struct i387_fxsave_struct * fx = ALIGN_UP(&(pad_fx.fx), 16);
	
	asm volatile (
		"fxsave	(%[fx])\n\t"
		: "=m" (*fx)
		: [fx] "cdaSDb" (fx)
	);

	*u = *fx;
}

/* according to u, restore reg state to r */
void
restore_reg_state(struct user_regs_struct * u,
		size_t size, struct pusha_regs * r, void ** p_eip)
{
	assert(u != NULL);
	assert(r != NULL);
	assert(size >= sizeof(*u));
	assert(p_eip != NULL);

	r->ebx = u->bx;
	r->ecx = u->cx;
	r->edx = u->dx;
	r->esi = u->si;
	r->edi = u->di;
	r->ebp = u->bp;
	r->eax = u->ax;
	*p_eip = (void*)u->ip;
	r->eflags = u->flags;
	r->esp = u->sp;

#define restorsr(s, r) asm volatile("movw %%ax, %%" #r : : "a" (s & 0xffff))
#if 0
	restorsr(u->cs, cs);
	restorsr(u->ds, ds);
	restorsr(u->es, es);
	restorsr(u->ss, ss);
#endif
	restorsr(u->fs, fs);
	restorsr(u->gs, gs);
#undef loadsr

}

void
restore_i387_state(struct i387_fxsave_struct * u, size_t size)
{
	assert(u != NULL);
	assert(size >= sizeof(*u));

	struct {
		struct i387_fxsave_struct fx;
		uint8_t __padding[16];
	} pad_fx;
	struct i387_fxsave_struct * fx = ALIGN_UP(&(pad_fx.fx), 16);

	*fx = *u;

	asm volatile (
			"fxrstor %[fx]\n\t"
			:
			: [fx] "m" (*fx));
}

// vim:ts=4:sw=4

