
#include "syscalls.h"


#define FDS_BITPERLONG	(8*sizeof(long))
#define FDS_LONGS(nr)	(((nr)+FDS_BITPERLONG-1)/FDS_BITPERLONG)
#define FDS_BYTES(nr)	(FDS_LONGS(nr)*sizeof(long))


#ifndef SYSCALL_PRINTER

int SCOPE
post__newselect(const struct syscall_regs * regs)
{
	write_eax(regs);
	int n = regs->ebx;
	uint32_t inp = regs->ecx;
	uint32_t outp = regs->edx;
	uint32_t exp = regs->esi;

	write_obj(n);
	write_obj(inp);
	write_obj(outp);
	write_obj(exp);

	int fd_bytes = FDS_BYTES(n);
	if (inp != 0)
		write_mem(inp, fd_bytes);
	if (outp != 0)
		write_mem(outp, fd_bytes);
	if (exp != 0)
		write_mem(exp, fd_bytes);
	return 0;
}

int SCOPE
replay__newselect(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	int n;
	uint32_t inp;
	uint32_t outp;
	uint32_t exp;

	read_obj(n);
	read_obj(inp);
	read_obj(outp);
	read_obj(exp);

	ASSERT(n == regs->ebx, regs, "");
	ASSERT(inp == regs->ecx, regs, "");
	ASSERT(outp == regs->edx, regs, "");
	ASSERT(exp == regs->esi, regs, "");

	int fd_bytes = FDS_BYTES(n);
	if (inp != 0)
		read_mem(inp, fd_bytes);
	if (outp != 0)
		read_mem(outp, fd_bytes);
	if (exp != 0)
		read_mem(exp, fd_bytes);
	return eax;
}

#else

void
output__newselect(void)
{
	int retval;
	retval = read_eax();
	printf("_newselect:\t0x%x\n", retval);

	int n;
	uint32_t inp, outp, exp;
	read_obj(n);
	read_obj(inp);
	read_obj(outp);
	read_obj(exp);

	int fd_bytes = FDS_BYTES(n);
	if (inp != 0)
		skip(fd_bytes);
	if (outp != 0)
		skip(fd_bytes);
	if (exp != 0)
		skip(fd_bytes);
}
#endif

