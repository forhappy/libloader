#ifndef __SIGNAL_DEFS_H__
#define __SIGNAL_DEFS_H__

/* related definitions from kernel code */

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t s32;

typedef s32		compat_timer_t;
typedef s32		compat_int_t;
typedef	u32		compat_uptr_t;
typedef s32		compat_clock_t;

typedef u32		__u32;
typedef s32		__s32;

#define SI_MAX_SIZE	128
#define SI_PAD_SIZE	((SI_MAX_SIZE - __ARCH_SI_PREAMBLE_SIZE) / sizeof(int))
#ifdef __x86_64__
# define __ARCH_SI_PREAMBLE_SIZE	(4 * sizeof(int))
#endif
#ifndef __ARCH_SI_PREAMBLE_SIZE
#define __ARCH_SI_PREAMBLE_SIZE	(3 * sizeof(int))
#endif

#ifndef __ARCH_SI_UID_T
#define __ARCH_SI_UID_T	uid_t
#endif
#define __ARCH_SI_BAND_T long

typedef int		__kernel_pid_t;
typedef long		__kernel_clock_t;

typedef unsigned int	__kernel_uid32_t;
typedef unsigned int	__kernel_gid32_t;

typedef __kernel_uid32_t	uid_t;
typedef __kernel_gid32_t	gid_t;
typedef __kernel_clock_t	clock_t;

typedef union sigval {
	int sival_int;
	void *sival_ptr;
} sigval_t;

struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpxreg {
	unsigned short significand[4];
	unsigned short exponent;
	unsigned short padding[3];
};

struct _xmmreg {
	unsigned long element[4];
};

struct _fpstate {
	/* Regular FPU environment */
	unsigned long	cw;
	unsigned long	sw;
	unsigned long	tag;
	unsigned long	ipoff;
	unsigned long	cssel;
	unsigned long	dataoff;
	unsigned long	datasel;
	struct _fpreg	_st[8];
	unsigned short	status;
	unsigned short	magic;		/* 0xffff = regular FPU data only */

	/* FXSR FPU environment */
	unsigned long	_fxsr_env[6];	/* FXSR FPU env is ignored */
	unsigned long	mxcsr;
	unsigned long	reserved;
	struct _fpxreg	_fxsr_st[8];	/* FXSR FPU reg data is ignored */
	struct _xmmreg	_xmm[8];
	unsigned long	padding[56];
};


typedef struct siginfo {
	int si_signo;
	int si_errno;
	int si_code;

	union {
		int _pad[SI_PAD_SIZE];

		/* kill() */
		struct {
			pid_t _pid;		/* sender's pid */
			__ARCH_SI_UID_T _uid;	/* sender's uid */
		} _kill;

		/* POSIX.1b timers */
		struct {
			timer_t _tid;		/* timer id */
			int _overrun;		/* overrun count */
			char _pad[sizeof( __ARCH_SI_UID_T) - sizeof(int)];
			sigval_t _sigval;	/* same as below */
			int _sys_private;       /* not to be passed to user */
		} _timer;

		/* POSIX.1b signals */
		struct {
			pid_t _pid;		/* sender's pid */
			__ARCH_SI_UID_T _uid;	/* sender's uid */
			sigval_t _sigval;
		} _rt;

		/* SIGCHLD */
		struct {
			pid_t _pid;		/* which child */
			__ARCH_SI_UID_T _uid;	/* sender's uid */
			int _status;		/* exit code */
			clock_t _utime;
			clock_t _stime;
		} _sigchld;

		/* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
		struct {
			void *_addr; /* faulting insn/memory ref. */
#ifdef __ARCH_SI_TRAPNO
			int _trapno;	/* TRAP # which caused the signal */
#endif
		} _sigfault;

		/* SIGPOLL */
		struct {
			__ARCH_SI_BAND_T _band;	/* POLL_IN, POLL_OUT, POLL_MSG */
			int _fd;
		} _sigpoll;
	} _sifields;
} siginfo_t;

struct sigcontext {
	unsigned short gs, __gsh;
	unsigned short fs, __fsh;
	unsigned short es, __esh;
	unsigned short ds, __dsh;
	unsigned long di;
	unsigned long si;
	unsigned long bp;
	unsigned long sp;
	unsigned long bx;
	unsigned long dx;
	unsigned long cx;
	unsigned long ax;
	unsigned long trapno;
	unsigned long err;
	unsigned long ip;
	unsigned short cs, __csh;
	unsigned long flags;
	unsigned long sp_at_signal;
	unsigned short ss, __ssh;
	struct _fpstate * fpstate;
	unsigned long oldmask;
	unsigned long cr2;
};

typedef unsigned int	__kernel_size_t;
typedef __kernel_size_t		size_t;

typedef struct sigaltstack {
	void *ss_sp;
	int ss_flags;
	size_t ss_size;
} stack_t;

struct ucontext {
	unsigned long	  uc_flags;
	struct ucontext  *uc_link;
	stack_t		  uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t	  uc_sigmask;	/* mask last for extensibility */
};

struct sigframe
{
	char *pretcode;
	int sig;
	struct sigcontext sc;
	struct _fpstate fpstate;
	unsigned long extramask[_NSIG_WORDS-1];
	char retcode[8];
};

struct rt_sigframe
{
	char *pretcode;
	int sig;
	struct siginfo *pinfo;
	void *puc;
	struct siginfo info;
	struct ucontext uc;
	struct _fpstate fpstate;
	char retcode[8];
};

#endif
