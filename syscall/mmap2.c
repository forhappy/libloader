
#include "syscall_tbl.h"
#include "log_and_ckpt.h"
#define PAGE_MASK	(~(PAGE_SIZE-1))
#define TASK_SIZE	(0xC0000000UL)

int SCOPE
post_mmap2(const struct syscall_regs * regs)
{
	write_eax(regs);
#if 0
	/* we have to check mmap, if it is a file map and successed,
	 * we must save its content, for replay use. */

	/* eax is result */
	uint32_t addr = regs->eax;
	uint32_t len = regs->ecx;
	uint32_t flags = regs->esi;

	if (!((addr & ~PAGE_MASK) || (addr > TASK_SIZE))) {
		/* check whether it is a file map */
		write_obj(flags);
		if (!(flags & MAP_ANONYMOUS)) {
			/* its a filemap, write memory */
			write_obj(len);
			/* no, don't write memory. */
			/* most of the time, file mapping is readonly */
			write_mem(addr, len);
		}
	}
#endif
	return 0;
}

