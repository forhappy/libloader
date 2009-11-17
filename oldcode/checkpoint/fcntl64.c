
#include "syscalls.h"
#define __ARCH_FLOCK64_PAD
typedef long long	__kernel_loff_t;
typedef __kernel_loff_t		k_loff_t;
struct flock64 {
	short  l_type;
	short  l_whence;
	k_loff_t l_start;
	k_loff_t l_len;
	pid_t  l_pid;
	__ARCH_FLOCK64_PAD
};

#define __ARCH_FLOCK_PAD
typedef long		__kernel_off_t;
typedef __kernel_off_t		k_off_t;
struct flock {
	short	l_type;
	short	l_whence;
	k_off_t	l_start;
	k_off_t	l_len;
	pid_t	l_pid;
	__ARCH_FLOCK_PAD
};
#define F_GETLK64	12	/*  using 'struct flock64' */
#define F_SETLK64	13
#define F_SETLKW64	14
#define F_GETLK		7

#ifndef SYSCALL_PRINTER
int SCOPE
post_fcntl64(const struct syscall_regs * regs)
{
	write_eax(regs);
	if (regs->eax >= 0) {
		uint32_t cmd = regs->ecx;
		uint32_t arg = regs->edx;
		write_obj(cmd);
		switch (cmd) {
			case F_GETLK64:
				write_mem(arg, sizeof(struct flock64));
				break;
			case F_GETLK:
				write_mem(arg, sizeof(struct flock));
				break;
			default:
				break;
		}
	}
	return 0;
}

int SCOPE
replay_fcntl64(const struct syscall_regs * regs)
{
	int32_t eax = read_int32();
	if (eax >= 0) {
		uint32_t cmd = read_uint32();
		ASSERT(cmd == regs->ecx, regs, "fcntl64 cmd inconsistent\n");
		uint32_t arg = regs->edx;
		switch (cmd) {
			case F_GETLK64:
				read_mem(arg, sizeof(struct flock64));
				break;
			case F_GETLK:
				read_mem(arg, sizeof(struct flock));
				break;
			default:
				break;
		}
	}
	return eax;
}

#else

void
output_fcntl64(int nr)
{
	int32_t eax = read_eax();
	printf("fcntl64:\t%d\n", eax);
	if (eax >= 0) {
		uint32_t cmd = read_uint32();
		switch (cmd) {
			case F_GETLK64:
				skip(sizeof(struct flock64));
				break;
			case F_GETLK:
				skip(sizeof(struct flock));
				break;
			default:
				break;
		}
	}
}
#endif

