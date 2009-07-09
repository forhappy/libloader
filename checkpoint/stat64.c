
#include "syscalls.h"

struct stat64 {
	unsigned long long	st_dev;
	unsigned char	__pad0[4];

	unsigned long	__st_ino;

	unsigned int	st_mode;
	unsigned int	st_nlink;

	unsigned long	st_uid;
	unsigned long	st_gid;

	unsigned long long	st_rdev;
	unsigned char	__pad3[4];

	long long	st_size;
	unsigned long	st_blksize;

	/* Number 512-byte blocks allocated. */
	unsigned long long	st_blocks;

	unsigned long	st_atime;
	unsigned long	st_atime_nsec;

	unsigned long	st_mtime;
	unsigned int	st_mtime_nsec;

	unsigned long	st_ctime;
	unsigned long	st_ctime_nsec;

	unsigned long long	st_ino;
};


#ifndef SYSCALL_PRINTER
int SCOPE
post_stat64(struct syscall_regs * regs)
{
	
	write_eax(regs);
	if (regs->eax >= 0) {
		write_mem(regs->ecx, sizeof(struct stat64));
	}
	return 0;
}

#else

void
output_stat64(void)
{
	int32_t ret = read_eax();
	if (ret >= 0)
		skip(sizeof(struct stat64));
	printf("stat64:\t%d\n", ret);
}
#endif

