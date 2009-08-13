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
typedef int		__kernel_timer_t;

typedef unsigned int	__kernel_uid32_t;
typedef unsigned int	__kernel_gid32_t;

typedef __kernel_uid32_t	uid_t;
typedef __kernel_gid32_t	gid_t;
typedef __kernel_clock_t	clock_t;
typedef __kernel_timer_t	k_timer_t;

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
			k_timer_t _tid;		/* timer id */
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

#define K_NSIG	(64)
#define _NSIG_WORDS	(2)

typedef struct {
	unsigned long sig[_NSIG_WORDS];
} k_sigset_t;

struct ucontext {
	unsigned long	  uc_flags;
	struct ucontext  *uc_link;
	stack_t		  uc_stack;
	struct sigcontext uc_mcontext;
	k_sigset_t	  uc_sigmask;	/* mask last for extensibility */
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

#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */

/* 
 * this k_sigaction is not the same as kernel's k_sigaction, which is:
 * struct k_sigaction {
 *	struct sigaction sa;
 * }
 * although they are equal internally.
 */
struct k_sigaction {
	void * sa_handler;
	unsigned long sa_flags;
	void * sa_restorer;
	k_sigset_t sa_mask;		/* mask last for extensibility */
};

#define _SIG_SET_BINOP(name, op)					\
static inline void name(k_sigset_t *r, const k_sigset_t *a, const k_sigset_t *b) \
{									\
	extern void _NSIG_WORDS_is_unsupported_size(void);		\
	unsigned long a0, a1, a2, a3, b0, b1, b2, b3;			\
									\
	switch (_NSIG_WORDS) {						\
	    case 4:							\
		a3 = a->sig[3]; a2 = a->sig[2];				\
		b3 = b->sig[3]; b2 = b->sig[2];				\
		r->sig[3] = op(a3, b3);					\
		r->sig[2] = op(a2, b2);					\
	    case 2:							\
		a1 = a->sig[1]; b1 = b->sig[1];				\
		r->sig[1] = op(a1, b1);					\
	    case 1:							\
		a0 = a->sig[0]; b0 = b->sig[0];				\
		r->sig[0] = op(a0, b0);					\
		break;							\
	    default:							\
		_NSIG_WORDS_is_unsupported_size();			\
	}								\
}

#define _sig_or(x,y)	((x) | (y))
_SIG_SET_BINOP(sigorsets, _sig_or)
#define _sig_and(x,y)	((x) & (y))
_SIG_SET_BINOP(sigandsets, _sig_and)
#define _sig_nand(x,y)	((x) & ~(y))
_SIG_SET_BINOP(signandsets, _sig_nand)

#ifdef __i386__
# define _NSIG_BPW	32
#else
# define _NSIG_BPW	64
#endif
static inline void sigaddset(k_sigset_t *set, int _sig)
{
	unsigned long sig = _sig - 1;
	if (_NSIG_WORDS == 1)
		set->sig[0] |= 1UL << sig;
	else
		set->sig[sig / _NSIG_BPW] |= 1UL << (sig % _NSIG_BPW);
}

static inline void sigaddsetmask(k_sigset_t *set, unsigned long mask)
{
	set->sig[0] |= mask;
}

static inline void sigdelsetmask(k_sigset_t *set, unsigned long mask)
{
	set->sig[0] &= ~mask;
}
#ifndef SIGKILL
# define SIGKILL (9)
#endif
#ifndef SIGSTOP
# define SIGSTOP (19)
#endif
#ifndef sigmask
# define sigmask(sig)	(1UL << ((sig) - 1))
#endif

extern SCOPE void
unhook_sighandlers(void);

#endif
