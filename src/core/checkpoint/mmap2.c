
#include "syscalls.h"
#include <sys/mman.h>
#ifdef IN_INJECTOR
# include "../libwrapper/injector.h"
#endif

#define PAGE_MASK	(~(PAGE_SIZE-1))
#define TASK_SIZE	(0xC0000000UL)

#ifndef SYSCALL_PRINTER
int SCOPE
post_mmap2(const struct syscall_regs * regs)
{
	write_eax(regs);
#if 0
	/* we have to check mmap, if it is a file ap and successed,
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

int SCOPE
replay_mmap2(const struct syscall_regs * regs)
{
	uint32_t eax;
	eax = read_uint32();
	/* eax is result */

	if (!((eax & ~PAGE_MASK) || (eax > TASK_SIZE))) {
		/* we must do the mmap for target */
#ifdef IN_INJECTOR
		uint32_t addr = eax;
		uint32_t len = regs->ecx;
		uint32_t prot = regs->edx;
		uint32_t flags = regs->esi;
		uint32_t fd = regs->edi;
		uint32_t pgoff = regs->ebp;


		uint32_t ret;
		/* it is a anon map */
		ret = INTERNAL_SYSCALL(mmap2, 6,
				addr, len, prot,
				flags | MAP_FIXED, fd, pgoff);
		ASSERT(ret == eax, regs, "mmap2 inconsistent\n");

#if 0

		/* DON'T INJECT MEMORY NOW */

		uint32_t lflags = read_uint32();
		ASSERT(lflags == flags, regs, "lflags=0x%x, flags=0x%x\n",
				lflags, flags);

		/* check if it is a file map */
		if ((flags & MAP_ANONYMOUS)) {
			uint32_t ret;
			/* it is a anon map */
			ret = INTERNAL_SYSCALL(mmap2, 6,
					addr, len, prot,
					flags | MAP_FIXED, fd, pgoff);
			ASSERT(ret == eax, regs, "");
		} else {
			/* it is a file map. the file shoule have
			 * been opened... */

			/* read len */
			int32_t llen;
			llen = read_uint32();
			ASSERT(len == llen, regs, "len=%d, llen=%d\n", len, llen);

			/* we must unset 'shared' flag */
			flags &= (~(MAP_SHARED));
			flags |= MAP_PRIVATE;
			flags |= MAP_FIXED;
			uint32_t ret;
			/* enable write */
			ret = INTERNAL_SYSCALL(mmap2, 6,
					addr, len, prot | PROT_WRITE,
					flags, fd, pgoff);
			ASSERT(ret == eax, regs, "\n");

			/* then inject memory */
			INJ_WARNING("len=0x%x, ret=0x%x, eax=0x%x\n", len, ret, eax);
			INJ_WARNING("0x%x\n", *(uint32_t*)(ret));

			read_mem(ret, len);
		}
#endif
#endif
	}
	return eax;
}


#else

void
output_mmap2(int nr)
{
	int32_t addr = read_eax();
	printf("mmap2:\t0x%x\n", addr);
#if 0
	if (!((addr & ~PAGE_MASK) || (addr > TASK_SIZE))) {
		uint32_t flags;
		read_obj(flags);
		if (!(flags & MAP_ANONYMOUS)) {
			uint32_t len;
			read_obj(len);
			skip(len);
		}
	}
#endif	
}
#endif

