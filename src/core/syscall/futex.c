
#include "syscall_tbl.h"
#include "log_and_ckpt.h"

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

