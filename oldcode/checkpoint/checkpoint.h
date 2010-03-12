/* 
 * recorder.h
 * by WN @ Jun. 22, 2009
 */

#ifndef __CHECKPOINT_H
#define __CHECKPOINT_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/user.h>
#include "defs.h"
#include "syscall_nr.h"

#ifndef IN_INJECTOR
# include "ptraceutils.h"
# include <stdio.h>
# include <stdlib.h>
# include <alloca.h>
# include "debug.h"
# include "exception.h"
#else
# include "injector.h"
# include "injector_utils.h"
# include "injector_debug.h"
#endif


__BEGIN_DECLS

#ifndef IN_INJECTOR
# define SCOPE
# define __open open
# define __close close
# define __read read
# define __write write
# define __lseek lseek
# define __printf printf
# define __exit(x) THROW(EXCEPTION_FATAL, "checkpoint exit")
# define __dup_mem(d, s, sz) ptrace_dupmem(d, s, sz)
# define __upd_mem(d, s, sz) ptrace_updmem((void*)s, d, sz)

/* temporarily define read_obj */
# define read_obj(x)	do {memset(&x, '\0', sizeof(x));} while(0)
# define read_mem(dst, sz)	do {memset((void*)dst, '\0', sz);} while(0)

extern int SCOPE logger_fd;

# define write_mem(addr, sz) do {	\
	void * p;			\
	if (sz <= (1 << 20))		\
		p = alloca(sz);		\
	else				\
		p = malloc(sz);		\
	__dup_mem(p, addr, sz);		\
	__write(logger_fd, p, sz);	\
	if (sz > (1 << 20))		\
		free(p);		\
} while(0)

# define __write_logger(__addr, __sz) do {	\
	__write(logger_fd, (__addr), (__sz));	\
} while(0)

#else	/* IN_INJECTOR */
# define __open(args...)	INTERNAL_SYSCALL(open, 3, args)
# define __close(args)		INTERNAL_SYSCALL(close, 1, args)
# define __read(args...)	INTERNAL_SYSCALL(read, 3, args)
# define __write(args...)	INTERNAL_SYSCALL(write, 3, args)
# define __printf(args...)	printf(args)
# define __lseek(fd, off, ori)	INTERNAL_SYSCALL(lseek, 3, fd, off, ori)
# define __exit(x)		INTERNAL_SYSCALL(exit, 1, x)

# define __dup_mem(d, s, sz)	memcpy((void*)d, (void*)s, sz)
# define __upd_mem(d, s, sz)	memcpy((void*)d, (void*)s, sz)

extern int SCOPE logger_fd;
/* size of logger file */
extern uint32_t SCOPE logger_sz;

# define __write_logger(__addr, __sz) do {	\
	int err;				\
	logger_sz += (__sz);			\
	err = __write(logger_fd, (__addr), (__sz)); \
	ASSERT(err == (__sz), regs, "write logger failed: %d != %d\n", err, (__sz));\
} while(0)

# define write_mem(addr, sz) do {	\
	__write_logger(addr, sz);	\
} while(0)


# define read_obj(x) do {	\
	int err;	\
	err = __read(logger_fd, &(x), sizeof(x));\
	ASSERT(err == sizeof(x), regs, "read failed: %d != %d\n", err, sizeof(x));\
} while(0)
#if 0
# define read_mem(dst, sz)	do {\
	int ret = 0;\
	INJ_FATAL("dst=0x%x, sz=%d\n", (dst), (sz));\
	ret = __read(logger_fd, (void*)(dst), (sz));	\
} while(0)
#endif

# define read_mem(dst, sz)	do {\
	int ___ret = 0;\
	___ret = __read(logger_fd, (void*)(dst), (sz));	\
	ASSERT(___ret == sz, regs, "read_mem failed: ___ret=%d, sz=%d\n", ___ret, sz);\
} while(0)

#endif	/* IN_INJECTOR */


#ifndef SEEK_SET
# define SEEK_SET	0	/* seek relative to beginning of file */
#endif
#ifndef SEEK_SET
# define SEEK_CUR	1	/* seek relative to current file position */
#endif
#ifndef SEEK_SET
# define SEEK_END	2	/* seek relative to end of file */
#endif

#ifndef SYSCALL_PRINTER
# define seek_logger(off, ori)	__lseek(logger_fd, off, ori)
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

#define GDT_ENTRY_TLS_MIN	6
#define GDT_ENTRY_TLS_ENTRIES 3
#define GDT_ENTRY_TLS_MAX 	(GDT_ENTRY_TLS_MIN + GDT_ENTRY_TLS_ENTRIES - 1)

struct user_desc {
	unsigned int  entry_number;
	unsigned int  base_addr;
	unsigned int  limit;
	unsigned int  seg_32bit:1;
	unsigned int  contents:2;
	unsigned int  read_exec_only:1;
	unsigned int  limit_in_pages:1;
	unsigned int  seg_not_present:1;
	unsigned int  useable:1;
};


struct robust_list {
	struct robust_list *next;
};

struct robust_list_head {
	struct robust_list list;
	long futex_offset;
	struct robust_list *list_op_pending;
};

struct i387_fxsave_struct {
	uint16_t	cwd;
	uint16_t	swd;
	uint16_t	twd;
	uint16_t	fop;
	union {
		struct {
			uint64_t	rip;
			uint64_t	rdp;
		};
		struct {
			uint32_t	fip;
			uint32_t	fcs;
			uint32_t	foo;
			uint32_t	fos;
		};
	};
	uint32_t	mxcsr;
	uint32_t	mxcsr_mask;
	uint32_t	st_space[32];	/* 8*16 bytes for each FP-reg = 128 bytes */
	uint32_t	xmm_space[64];	/* 16*16 bytes for each XMM-reg = 256 bytes */
	uint32_t	padding[24];
};

#include "libwrapper/signal_defs.h"
/* struct i387_fxsave_struct will be embed into state vector, don't
 * use aligned option here. use it in defenition */

extern SCOPE struct state_vector {
	int dummy;
	uint32_t brk;
	pid_t pid;
	uint32_t clear_child_tid;
	uint32_t robust_list;
	struct user_desc thread_area[GDT_ENTRY_TLS_ENTRIES];
	/* signal 0 is no use */
	struct k_sigaction sigactions[K_NSIG+1];
	k_sigset_t sigmask;
	/* add user regs into state_vector, this is for only
	 * checkpoints use */
	struct user_regs_struct regs;
	struct i387_fxsave_struct fpustate;
	int end;
} state_vector;

extern SCOPE struct injector_opts {
	int logger_threshold;
	int trace_fork;
	int untraced;
	int nowait;	/* for gdbloader only */
} injector_opts;


struct mem_region {
	uint32_t start;
	uint32_t end;
	uint32_t prot;
	uint32_t offset;
	uint32_t f_pos;	/* start in ckpt file */
	uint32_t fn_len;
	char fn[0];
};

extern SCOPE int
checkpoint_init(void);

#include "checkpoint/syscall_table.h"

#ifdef SYSCALL_PRINTER
# include <stdio.h>
extern void
read_logger(void * buffer, int sz);

extern void
seek_logger(int off, int origin);

#ifdef read_obj
# undef read_obj
#endif

#define read_obj(x) read_logger(&(x), sizeof(x))
#define read_regs(r)	read_obj(r)

#ifdef read_mem
# undef read_mem
#endif

#define read_mem(d, sz)	do {	\
	read_logger(d, sz);	\
} while(0)

#define skip(n)	read_logger(NULL, n)

#endif

#define CKPT_MAGIC	(0x54504b43)

#ifdef read_obj
# define read_eax() ({int32_t eax; read_obj(eax); eax;})
# define read_int32() ({int32_t v; read_obj(v); v;})
# define read_uint32() ({uint32_t v; read_obj(v); v;})
# define read_int16() ({int16_t v; read_obj(v); v;})
#endif

/* caller must ensure fx is aligned by 16 bytes */
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

struct seg_regs {
	uint16_t cs, ds, es, fs, gs, ss;
};

#ifdef IN_INJECTOR

static inline void ATTR(noreturn)
replay_trap(const struct syscall_regs * regs)
{
	INJ_FATAL("eip=0x%x\n", regs->eip);
	INJ_FATAL("esp=0x%x\n", regs->esp);
	INJ_FATAL("ebp=0x%x\n", regs->ebp);
	asm volatile (
			"movl %0, %%esp\n"
			"movl %1, %%ebp\n" : : "m" (regs->esp), "m" (regs->ebp));
	asm volatile ("int3\n");
	__exit(-1);
	while(1);
}



extern struct i387_fxsave_struct SCOPE fpustate_struct;
extern SCOPE void
make_checkpoint(struct syscall_regs * r,
		struct i387_fxsave_struct * fpustate,
		struct seg_regs * seg_regs,
		int keep_old);
#endif

extern SCOPE int
logger_init(pid_t pid);

extern SCOPE int
logger_close(void);

extern SCOPE int
before_syscall(const struct syscall_regs * regs);

extern SCOPE int
after_syscall(const struct syscall_regs * regs);

SCOPE uint32_t
replay_syscall(const struct syscall_regs * regs);

extern SCOPE char *
readline(int fd);

__END_DECLS

#endif

