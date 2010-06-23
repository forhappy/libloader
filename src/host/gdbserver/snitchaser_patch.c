/* 
 * snitchaser_patch.c
 * by WN @ Jun. 16, 2010
 */

#include <common/debug.h>
#include <host/exception.h>

#include <host/gdbserver/snitchaser_patch.h>
#include <host/arch_replayer_helper.h>
#include <host/replay_log.h>
#include <asm_offsets.h>
#include <xasm/processor.h>

#include <sys/ptrace.h>

/* target.h depends on server.h */
#include "server.h"
#include "target.h"

struct SN_info SN_info;

static struct pusha_regs * pusha_regs_addr;

static void
target_read_memory(void * memaddr, void * myaddr, size_t len)
{
	TRACE(XGDBSERVER, "read memory: %p %d\n", memaddr, len);
	assert(the_target->read_memory);
	assert(myaddr != NULL);
	int err = read_inferior_memory((CORE_ADDR)(uintptr_t)memaddr, myaddr, len);
	if (err != 0)
		THROW_FATAL(EXP_GDBSERVER_ERROR,
				"read inferior memory error: %d", err);
}

void
SN_init(void)
{
	pusha_regs_addr = SN_info.stack_base + OFFSET_PUSHA_REGS;
}

void
SN_reset_registers(void)
{
	/* first: retrieve registers from tls stack */
	struct pusha_regs regs;

	/* the esp in regs is adjusted by replayer */
	target_read_memory((void*)pusha_regs_addr, &regs, sizeof(regs));

	TRACE(XGDBSERVER, "got registers:\n");
	TRACE(XGDBSERVER, "\teax=0x%x\n", regs.eax);
	TRACE(XGDBSERVER, "\tebx=0x%x\n", regs.ebx);
	TRACE(XGDBSERVER, "\tecx=0x%x\n", regs.ecx);
	TRACE(XGDBSERVER, "\tedx=0x%x\n", regs.edx);
	TRACE(XGDBSERVER, "\tesi=0x%x\n", regs.esi);
	TRACE(XGDBSERVER, "\tedi=0x%x\n", regs.edi);
	TRACE(XGDBSERVER, "\tesp=0x%x\n", regs.esp);
	TRACE(XGDBSERVER, "\tebp=0x%x\n", regs.ebp);

	void * eip;
	target_read_memory(SN_info.stack_base + OFFSET_TARGET,
			&eip, sizeof(eip));
	TRACE(XGDBSERVER, "\teip=%p\n", eip);

	/* restore registers */
	arch_restore_registers(SN_info.pid, &regs, eip);
}


static void
ptrace_cont(struct user_regs_struct * saved_regs)
{
	TRACE(XGDBSERVER, "ptrace_cont\n");
}

static void
ptrace_single_step(struct user_regs_struct * saved_regs)
{
	TRACE(XGDBSERVER, "ptrace_singlestep\n");
	/* fetch original eip */
}

int
SN_ptrace_cont(enum __ptrace_request req, pid_t pid,
		uintptr_t addr, uintptr_t data)
{
	assert((req == PTRACE_CONT) || (req == PTRACE_SINGLESTEP));
	if (pid != SN_info.pid)
		return ptrace(req, pid, addr, data);

	struct user_regs_struct saved_urs;
	if (req == PTRACE_SINGLESTEP)
		ptrace_single_step(&saved_urs);
	else
		ptrace_cont(&saved_urs);

#if 0
	/* get current eip, put it into OFFSET_TARGET, then redirect
	 * code into SN_info.patch_block_func */

	uintptr_t ptr = read_ptr_from_log();
	if (ptr < 0x1000) {
		/* this is system call */
		THROW_FATAL(EXP_UNIMPLEMENTED, "system call %d\n", ptr);
	}

	if (ptr > 0xc0000000) {
		/* this is mark */
		THROW_FATAL(EXP_UNIMPLEMENTED, "mark 0x%x\n", ptr);
	}

	TRACE(XGDBSERVER, "next branch target is 0x%x\n", ptr);

	redirect_

#endif
	THROW_FATAL(EXP_UNIMPLEMENTED, "xxxxx");
	return ptrace(req, pid, addr, data);
}

// vim:ts=4:sw=4

