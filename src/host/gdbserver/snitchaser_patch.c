/* 
 * snitchaser_patch.c
 * by WN @ Jun. 16, 2010
 */

#include <common/debug.h>
#include <host/exception.h>

#include <host/gdbserver/snitchaser_patch.h>
#include <host/arch_replayer_helper.h>
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


int
SN_ptrace_cont(enum __ptrace_request req, pid_t pid,
		uintptr_t addr, uintptr_t data)
{
	assert((req == PTRACE_CONT) || (req == PTRACE_SINGLESTEP));
	if (pid != SN_info.pid)
		return ptrace(req, pid, addr, data);

	/* get current eip, put it into OFFSET_TARGET, then redirect
	 * code into SN_info.patch_block_func */

#warning stop here, working on uncompression of logging

	return ptrace(req, pid, addr, data);
}

// vim:ts=4:sw=4

