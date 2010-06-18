/* 
 * snitchaser_patch.c
 * by WN @ Jun. 16, 2010
 */

#include <common/debug.h>
#include <host/exception.h>

#include <host/gdbserver/snitchaser_patch.h>
#include <asm_offsets.h>
#include <xasm/processor.h>

/* target.h depends on server.h */
#include "server.h"
#include "target.h"

pid_t SN_target_original_pid;
pid_t SN_target_original_tid;
pid_t SN_target_pid;
int SN_target_tnr;
void * SN_target_stack_base;

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
SN_init(pid_t ori_pid, pid_t ori_tid,
		pid_t pid, int tnr, void * stack_base)
{
	SN_target_original_pid = ori_pid;
	SN_target_original_tid = ori_tid;
	SN_target_pid = pid;
	SN_target_tnr = tnr;
	SN_target_stack_base = stack_base;

	pusha_regs_addr = SN_target_stack_base + OFFSET_PUSHA_REGS;
}

void
SN_reset_registers(void)
{
	/* first: retrieve registers from tls stack */
	struct pusha_regs regs;
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
	THROW_VAL(EXP_GDBSERVER_EXIT, 1, "xxxx");
}

// vim:ts=4:sw=4

