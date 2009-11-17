
#include "syscalls.h"

#define FUTEX_WAIT		0
#define FUTEX_WAKE		1
#define FUTEX_FD		2
#define FUTEX_REQUEUE		3
#define FUTEX_CMP_REQUEUE	4
#define FUTEX_WAKE_OP		5
#define FUTEX_LOCK_PI		6
#define FUTEX_UNLOCK_PI		7
#define FUTEX_TRYLOCK_PI	8
#define FUTEX_WAIT_BITSET	9
#define FUTEX_WAKE_BITSET	10

#define FUTEX_PRIVATE_FLAG	128
#define FUTEX_CMD_MASK		~FUTEX_PRIVATE_FLAG


#ifndef SYSCALL_PRINTER

int SCOPE
post_futex(const struct syscall_regs * regs)
{
	int32_t eax = regs->eax;
	write_eax(regs);

	if (eax < 0)
		return 0;

	uint32_t uaddr = regs->ebx;
	int op = regs->ecx;
	uint32_t uaddr2 =regs->edi;

	int cmd = op & FUTEX_CMD_MASK;

	write_obj(cmd);

	/* write uaddr */
	write_mem(uaddr, sizeof(uint32_t));
	switch (cmd) {
		case FUTEX_REQUEUE:
		case FUTEX_CMP_REQUEUE:
		case FUTEX_WAKE_OP:
			write_mem(uaddr2, sizeof(uint32_t));
		default:
			break;
	}
	return 0;
}

int SCOPE
replay_futex(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax < 0)
		return eax;

	uint32_t uaddr = regs->ebx;
	int op = regs->ecx;
	uint32_t uaddr2 =regs->edi;

	int cmd;
	read_obj(cmd);
	ASSERT(cmd == (op & FUTEX_CMD_MASK), regs, 
			"futex cmd inconsistent: should be 0x%x, actually 0x%x\n",
			cmd, op & FUTEX_CMD_MASK);

	/* read uaddr */
	read_mem(uaddr, sizeof(uint32_t));
	switch (cmd) {
		case FUTEX_REQUEUE:
		case FUTEX_CMP_REQUEUE:
		case FUTEX_WAKE_OP:
			read_mem(uaddr2, sizeof(uint32_t));
		default:
			break;
	}

	return eax;
}
#else

void
output_futex(int nr)
{
	int eax = read_eax();
	printf("futex:\t0x%x\n", eax);
	if (eax < 0)
		return;
	uint32_t cmd = read_uint32();
	skip(sizeof(uint32_t));

	switch (cmd) {
		case FUTEX_REQUEUE:
		case FUTEX_CMP_REQUEUE:
		case FUTEX_WAKE_OP:
			skip(sizeof(uint32_t));
		default:
			break;
	}
}
#endif

