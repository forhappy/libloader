/* 
 * x86/interp/arch_checkpoint.c
 * by WN @ Nov. 03, 2010
 */

#include <asm/user.h>
#include <asm/processor.h>
#include <asm/ldt.h>
#include <xasm/processor.h>
#include <xasm/syscall.h>
#include <common/defs.h>
#include <common/debug.h>
#include <interp/arch_checkpoint.h>

struct arch_cpuinfo_s {
	enum arch_ckpt_mark umark;
	size_t usz;
	struct user_regs_struct u;
	enum arch_ckpt_mark fxmark;
	size_t fxsz;
	struct i387_fxsave_struct fx;
};

size_t
get_ckpt_cpuinfo(void * buffer, size_t sz,
		struct pusha_regs * regs, void * eip)
{
	struct user_regs_struct u;
	struct i387_fxsave_struct fx;

	struct arch_cpuinfo_s * p_arch_ckpt;

	assert(buffer != NULL);
	assert(sz >= sizeof(*p_arch_ckpt));

	p_arch_ckpt = buffer;

	p_arch_ckpt->umark = ARCH_CKPT_SECT_UREGS_MARK;
	p_arch_ckpt->usz = sizeof(p_arch_ckpt->u);
	save_reg_state(&p_arch_ckpt->u, sizeof(u), regs, eip);

	p_arch_ckpt->fxmark = ARCH_CKPT_SECT_FXREGS_MARK;
	p_arch_ckpt->fxsz = sizeof(p_arch_ckpt->fxsz);
	save_i387_state(&p_arch_ckpt->fx, sizeof(fx));

	return sizeof(*p_arch_ckpt);
}

void
restore_ckpt_cpuinfo(void * buffer, size_t sz,
		struct pusha_regs * regs, void ** p_eip)
{
	struct user_regs_struct u;
	struct i387_fxsave_struct fx;
	struct arch_cpuinfo_s * p_arch_ckpt;
	assert(buffer != NULL);
	assert(sz == sizeof(*p_arch_ckpt));

	p_arch_ckpt = buffer;
	assert(p_arch_ckpt->umark == ARCH_CKPT_SECT_UREGS_MARK);
	assert(p_arch_ckpt->usz == sizeof(p_arch_ckpt->u));
	restore_reg_state(&p_arch_ckpt->u, sizeof(u), regs, p_eip);

	assert(p_arch_ckpt->fxmark == ARCH_CKPT_SECT_FXREGS_MARK);
	assert(p_arch_ckpt->fxsz == sizeof(p_arch_ckpt->fxsz));
	restore_i387_state(&p_arch_ckpt->fx, sizeof(fx));
}

static int
check_arch_gdt_min(struct user_desc * pd)
{
	int err;
	int min = 0;
	struct user_desc d;
	for (min = 0; min < 64 - GDT_ENTRY_TLS_ENTRIES; min++) {
		memset(&d, '\0', sizeof(d));
		d.entry_number = min;
		err = sys_get_thread_area(&d);
		if (err == 0) {
			DEBUG(CKPT, "GDT_ENTRY_TLS_MIN=%d\n", min);
			break;
		}

		if (err != -EINVAL)
			FATAL(CKPT, "get_thread_area returns %d\n", err);
	}
	if (min >= 64)
		FATAL(CKPT, "unable to get GDT_ENTRY_TLS_MIN\n");
	if (pd != NULL)
		*pd = d;
	return min;
}

#define TLS_INFO_SZ (sizeof(struct user_desc) * GDT_ENTRY_TLS_ENTRIES)
size_t
get_tls_info(void * buffer, size_t sz)
{
	assert(buffer != NULL);
	assert(sz >= TLS_INFO_SZ);
	struct user_desc * u_info = buffer;

	/* find the first entry */
	int err;
	int min = check_arch_gdt_min(&u_info[0]);
	for (int i = 1; i < GDT_ENTRY_TLS_ENTRIES; i++) {
		u_info[i].entry_number = min + i;
		err = sys_get_thread_area(&(u_info[i]));
		assert(err == 0);
	}

	return TLS_INFO_SZ;
}

void
restore_tls_info(void * buffer, size_t sz)
{
	assert(buffer != NULL);
	assert(sz == TLS_INFO_SZ);

	struct user_desc * u_info = buffer;
	int gdt_min = check_arch_gdt_min(NULL);

	for (int i = 0; i < GDT_ENTRY_TLS_ENTRIES; i++) {
		int err;
		assert(u_info[i].entry_number == gdt_min + i);
		err = sys_set_thread_area(&(u_info[i]));
		assert(err == 0);
	}
}


// vim:ts=4:sw=4

