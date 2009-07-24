
#include "syscalls.h"
#include <sys/mman.h>
#ifdef IN_INJECTOR
# include "injector.h"
#endif

#define PAGE_MASK	(~(PAGE_SIZE-1))
#define TASK_SIZE	(0xC0000000UL)

#ifndef SYSCALL_PRINTER
int SCOPE
post_mmap2(const struct syscall_regs * regs)
{
	write_eax(regs);
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
			write_mem(addr, len);
		}
	}
	return 0;
}

int SCOPE
replay_mmap2(const struct syscall_regs * regs)
{
	uint32_t eax;
	eax = read_uint32();
	INJ_WARNING("XXXX, eax=0x%x\n", eax);
	INJ_WARNING("XXXX, ebx=0x%x\n", regs->ebx);
	INJ_WARNING("XXXX, ecx=0x%x\n", regs->ecx);
	INJ_WARNING("XXXX, edx=0x%x\n", regs->edx);
	INJ_WARNING("XXXX, esi=0x%x\n", regs->esi);
	INJ_WARNING("XXXX, edi=0x%x\n", regs->edi);
	INJ_WARNING("XXXX, ebp=0x%x\n", regs->ebp);

	/* eax is result */

	if (!((eax & ~PAGE_MASK) || (eax > TASK_SIZE))) {
		INJ_WARNING("Try...\n");
		/* we must do the mmap for target */
#ifdef IN_INJECTOR
		uint32_t addr = eax;
		uint32_t len = regs->ecx;
		uint32_t prot = regs->edx;
		uint32_t flags = regs->esi;
		uint32_t fd = regs->edi;
		uint32_t pgoff = regs->ebp;
		/* check if it is a file map */
		if ((flags & MAP_ANONYMOUS)) {
			uint32_t ret;
			/* it is a anon map */
			ret = INTERNAL_SYSCALL(mmap2, 6,
					addr, len, prot,
					flags | MAP_FIXED, fd, pgoff);
			ASSERT(ret == eax, "");
		} else {
			/* it is a file map. the file shoule have
			 * been opened... */
			/* we must unset 'shared' flag */
			flags &= (~(MAP_SHARED));
			flags |= MAP_PRIVATE;
			flags |= MAP_FIXED;
			uint32_t ret;
			ret = INTERNAL_SYSCALL(mmap2, 6,
					addr, len, prot,
					flags, fd, pgoff);
			ASSERT(ret == eax, "");

			/* the inject memory */
			read_mem(ret, len);
		}
#endif
	}
	return eax;
}


#else

void
output_mmap2(void)
{
	int32_t addr = read_eax();
	printf("mmap2:\t0x%x\n", addr);
	if (!((addr & ~PAGE_MASK) || (addr > TASK_SIZE))) {
		uint32_t flags;
		read_obj(flags);
		if (!(flags & MAP_ANONYMOUS)) {
			uint32_t len;
			read_obj(len);
			skip(len);
		}
	}
	
}
#endif

