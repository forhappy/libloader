/*
 * =====================================================================================
 *
 *       Filename:  log_and_ckpt.h
 *
 *    Description:  write syscall number and its returned value into log file,
 *    and make checkpoint file.  
 *
 *        Version:  1.0
 *        Created:  04/12/2011 04:40:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  HP.Fu 
 *        Company:  ICT
 *
 * =====================================================================================
 */
#ifndef __LOG_AND_CKPT_H
#define __LOG_AND_CKPT_H


#include <debug.h>
#include <syscall.h>
#include <linux/string.h>
#include <loader/processor.h>
__BEGIN_DECLS

extern int SCOPE logger_fd;
/* size of logger file */
extern uint32_t SCOPE logger_sz;

# define __write_logger(__addr, __sz) do {	\
	int err;				\
	logger_sz += (__sz);			\
	err = sys_write(logger_fd, (__addr), (__sz)); \
	CASSERT(err == (__sz), LOG_SYSCALL, "write logger failed: %d != %d\n", err, (__sz));\
} while(0)

# define write_mem(addr, sz) do {	\
	__write_logger(addr, sz);	\
} while(0)

#ifndef SEEK_SET
# define SEEK_SET	0	/* seek relative to beginning of file */
#endif
#ifndef SEEK_SET
# define SEEK_CUR	1	/* seek relative to current file position */
#endif
#ifndef SEEK_SET
# define SEEK_END	2	/* seek relative to end of file */
#endif


# define write_syscall_nr(nr)	do {	\
	uint16_t x_nr;	\
	x_nr = (nr);	\
	__write_logger(&(x_nr), sizeof(x_nr));	\
} while(0)

# define write_regs(regs)	do {	\
	__write_logger((regs), sizeof(*(regs)));	\
} while(0)

# define write_eax(regs)	do {	\
	__write_logger(&((regs)->eax), sizeof((regs)->eax));	\
} while(0)

# define write_int32(x) do { \
	int32_t d = (x);	\
	__write_logger(&d, sizeof(d));\
} while(0)

# define write_int16(x) do { \
	int16_t d = (x);	\
	__write_logger(&d, sizeof(d));\
} while(0)

# define write_obj(x) do { \
	__write_logger(&(x), sizeof(x));\
} while(0)

static void inline
save_i387(struct i387_fxsave_struct * fx)
{
	asm volatile (
		"fxsave	(%[fx])\n\t"
		: "=m" (*fx)
		: [fx] "cdaSDb" (fx)
	);
}

static void inline
restore_i387(struct i387_fxsave_struct * fx)
{
	asm volatile (
		"fxrstor (%[fx])\n\t"
		:
		: [fx] "cdaSDb" (fx)
	);
}
__END_DECLS
#endif
