/* 
 * main.c
 * by WN @ Oct. 14, 2010
 *
 * the entry of libinterp.so
 */

#include <config.h>
#include <defs.h>
#include <debug.h>
#include <syscall.h>

#include <linux/kernel.h>
#include <linux/personality.h>

#include <loader/startup_stack.h>
#include <loader/env_conf.h>
#include <loader/processor.h>
#include <loader/interp_main.h>
#include <loader/elf.h>
/* interp/startup.S use a trick to refer '_start' symbol.
 * we do this to hide asm in platform independent code */
/* see startup.S. define INTERP_START_ENTRY to add '_start' entry */

//#define INTERP_START_ENTRY
//#include "startup.S"
//

#ifndef __ASSEMBLY__

#define INTERP_RET_UNSELECTED	(0)
#define INTERP_RET_SNITCHASER   (1)


typedef struct
{
  uint32_t a_type;		/* Entry type */
  union
    {
      uint32_t a_val;		/* Integer value */
      /* We use to have pointer elements added here.  We cannot do that,
	 though, since it does not work when using 32-bit definitions
	 on 64-bit platforms and vice versa.  */
    } a_un;
} Elf32_auxv_t;


#define AT_NULL		0		/* End of vector */
#define AT_IGNORE	1		/* Entry should be ignored */
#define AT_EXECFD	2		/* File descriptor of program */
#define AT_PHDR		3		/* Program headers for program */
#define AT_PHENT	4		/* Size of program header entry */
#define AT_PHNUM	5		/* Number of program headers */
#define AT_PAGESZ	6		/* System page size */
#define AT_BASE		7		/* Base address of interpreter */
#define AT_FLAGS	8		/* Flags */
#define AT_ENTRY	9		/* Entry point of program */
#define AT_NOTELF	10		/* Program is not ELF */
#define AT_UID		11		/* Real uid */
#define AT_EUID		12		/* Effective uid */
#define AT_GID		13		/* Real gid */
#define AT_EGID		14		/* Effective gid */
#define AT_CLKTCK	17		/* Frequency of times() */

/* Some more special a_type values describing the hardware.  */
#define AT_PLATFORM	15		/* String identifying platform.  */
#define AT_HWCAP	16		/* Machine dependent hints about
					   processor capabilities.  */

/* This entry gives some information about the FPU initialization
   performed by the kernel.  */
#define AT_FPUCW	18		/* Used FPU control word.  */

/* Cache block sizes.  */
#define AT_DCACHEBSIZE	19		/* Data cache block size.  */
#define AT_ICACHEBSIZE	20		/* Instruction cache block size.  */
#define AT_UCACHEBSIZE	21		/* Unified cache block size.  */

/* A special ignored value for PPC, used by the kernel to control the
   interpretation of the AUXV. Must be > 16.  */
#define AT_IGNOREPPC	22		/* Entry should be ignored.  */

#define	AT_SECURE	23		/* Boolean, was exec setuid-like?  */

#define AT_BASE_PLATFORM 24		/* String identifying real platforms.*/

#define AT_RANDOM	25		/* Address of 16 random bytes.  */

#define AT_EXECFN	31		/* Filename of executable.  */

/* Pointer to the global system page used for system calls and other
   nice things.  */
#define AT_SYSINFO	32
#define AT_SYSINFO_EHDR	33

#ifdef INTERP_START_ENTRY
extern int interp_start[] ATTR_HIDDEN;
extern int _start[] ATTR_HIDDEN;


extern void
wrapped_syscall();

extern void 
wrapped_syscall_entry();


asm (".globl _start\n"
	"_start:\n"
	"jmp interp_start\n");

/* use volatile to prevent optmization */
struct interp_startup_stack {
	volatile struct pusha_regs saved_regs;
	void * volatile stack_top;
};

#define SET_INTERP_RETADDR(stktop, addr)	\
	(*((void**)((uintptr_t)(stktop) - 4)) = (addr))
#endif

struct snitchaser_startup_stack {
	volatile struct pusha_regs saved_regs;
	void * volatile retaddr;
};

asm (
		".text\n\
		.globl interp_start\n\
		.hidden interp_start\n\
		interp_start:\n\
		push $0\n\
		push %esp\n\
		addl $4, (%esp)\n\
		pusha\n\
		pushf\n\
		pushl %esp\n\
		call xmain\n\
		addl $4, %esp\n\
		cmpl $1, %eax\n\
		je snitchaser_start\n\
		cmpl $0, %eax\n\
		je unselect_start\n\
		int3\n\
	unselect_start:\n\
		popf\n\
		popa\n\
		movl (%esp), %esp\n\
		addl $-4, %esp\n\
		ret\n\
	snitchaser_start:\n\
		popf\n\
		popa\n\
		movl (%esp), %esp\n\
		addl $-4, %esp\n\
		pusha\n\
		pushf\n\
		push %esp\n\
		call snitchaser_main\n\
		addl $4, %esp\n\
		popf\n\
		popa\n\
		ret\n\
		"
		);
#endif

unsigned long vsyscall_entry;

static void reexec(void);

void
set_vsyscall_entry(const char **envp, unsigned long new_entry)
{

    Elf32_auxv_t *auxv;
    while (*envp++ != NULL);
    /* find ELF auxiliary vectors (if this was an ELF binary) */
    auxv = (Elf32_auxv_t *) envp;

    for ( ; auxv->a_type != AT_NULL; auxv++)
        if (auxv->a_type == AT_SYSINFO) {
			VERBOSE(LOADER, "AT_SYSINFO entry at 0x%x, vdso entry:0x%x\n", 
					(unsigned int)&auxv, auxv->a_un.a_val);
			auxv->a_un.a_val = new_entry;
		}
}

unsigned int 
get_vsyscall_entry(const char **envp)
{
    Elf32_auxv_t *auxv;
    while (*envp++ != NULL);
    /* find ELF auxiliary vectors (if this was an ELF binary) */
    auxv = (Elf32_auxv_t *) envp;

    for ( ; auxv->a_type != AT_NULL; auxv++)
        if (auxv->a_type == AT_SYSINFO) {
			VERBOSE(LOADER, "AT_SYSINFO entry at 0x%x, vdso entry:0x%x\n", 
					(unsigned int)&auxv, auxv->a_un.a_val);
            return auxv->a_un.a_val;
		}
    TRACE(LOADER, stderr, "no AT_SYSINFO auxv entry found\n");
    sys_exit(1);
}

static void
reexec(void)
{
	/* check personality */
	TRACE(LOADER, "checking personality\n");
	int err = sys_personality(0xffffffff);
	assert(err >= 0);
	TRACE(LOADER, "personality: 0x%x\n", err);
	if (err & ADDR_NO_RANDOMIZE) {
		DEBUG(LOADER, "ADDR_NO_RANDOMIZE is set\n");
		return;
	}
	/* begin reexec */
	DEBUG(LOADER, "ADDR_NO_RANDOMIZE is not set\n");

	int persona = err | ADDR_NO_RANDOMIZE;
	err = sys_personality(persona);
	assert(err >= 0);

	const char ** new_argv = STACK_INFO(p_args);
	const char ** new_envs = STACK_INFO(p_envs);

	err = sys_execve(new_argv[0], new_argv, new_envs);

	FATAL(LOADER, "execve failed with %d\n", err);
}

static void
usage(void)
{
	FATAL(SYSTEM, "Usage: %s EXECUTABLE-FILE [ARGS-FOR-PROGRAM...]\n",
			STACK_INFO(p_args)[0]);
}

/* 
 * adjust stack when load exec
 */

static bool_t
load_exec(struct interp_startup_stack * startup_stack, void ** p_entry)
{
	/* shall we load real-exec? */
	/* hidden symbol will be retrived using ebx directly,
	 * avoid to generate R_386_GLOB_DAT relocation */
	extern int _start[] ATTR_HIDDEN;

	assert(STACK_AUXV_INFO(pp_user_phdrs) != NULL);
	assert(STACK_AUXV_INFO(p_nr_user_phdrs) != NULL);
	assert(STACK_AUXV_INFO(p_user_entry) != NULL);

	if (_start != *(STACK_AUXV_INFO(p_user_entry))) {
		DEBUG(LOADER, "loader is called by interp\n");
		if (p_entry != NULL)
			*p_entry = *(STACK_AUXV_INFO(p_user_entry));
		return TRUE;
	}

	const char * fn = STACK_INFO(p_args)[1];
	if (fn == NULL)
		usage();
	DEBUG(LOADER, "loading executable %s\n", fn);

	bool_t need_interp = FALSE;
	void * entry = load_elf(fn, NULL,
			STACK_AUXV_INFO(pp_user_phdrs),
			STACK_AUXV_INFO(p_nr_user_phdrs),
			&need_interp);
	DEBUG(LOADER, "executable %s loaded, entry at %p\n",
			fn, entry);

	assert(entry != NULL);

	*STACK_AUXV_INFO(p_execfn) = fn;
	*STACK_AUXV_INFO(p_user_entry) = entry;

	/* adjust stack: we are loaded by loader,
	 * remove the topmost argv */
	int argc = *STACK_INFO(p_argc);
	assert(argc >= 2);
	STACK_INFO(p_argc) = (void*)(&STACK_INFO(p_args)[0]);
	*STACK_INFO(p_argc) = argc - 1;
	STACK_INFO(p_args) = (void*)(&STACK_INFO(p_args)[1]);

	startup_stack->stack_top = STACK_INFO(p_argc);

	if (p_entry != NULL)
		*p_entry = entry;
	return need_interp;
}

static void
load_real_interp(struct interp_startup_stack * startup_stack, void ** p_entry)
{
	DEBUG(LOADER, "loading interp: %s\n", INTERP_FILE);
	void * entry = load_elf(REAL_INTERPRETER_FILE, NULL, NULL, NULL, NULL);
	assert(entry != NULL);

	if (p_entry != NULL)
		*p_entry = entry;
}

static int
sampling(void)
{
	bool_t selected = FALSE;
	int selection = random32() % 1000;
	if (CONF_VAL(sampling) < 0) {
		DEBUG(LOADER, "selected by negative sampling opt\n");
		selected = TRUE;
	} else if (selection < CONF_VAL(sampling)) {
		DEBUG(LOADER, "selected by sampling\n");
		selected = TRUE;
	} else {
		selected = FALSE;
	}

	if (!selected) {
		DEBUG(LOADER, "interp unselected\n");
		return INTERP_RET_UNSELECTED;
	}

	/* service(s) */
	if (CONF_VAL(use_rebranch)) {
		DEBUG(LOADER, "snitchaser selected\n");
		return INTERP_RET_SNITCHASER;
	}

	DEBUG(LOADER, "finally unselected\n");
	return INTERP_RET_UNSELECTED;
}


__attribute__((used, unused, visibility("hidden"))) int
xmain(struct interp_startup_stack * startup_stack)
{

	relocate_interp();
	dbg_init();


	TRACE(SYSTEM, "relocation OK\n");
	TRACE(SYSTEM, "stack top at %p\n", startup_stack->stack_top);

	scan_startup_stack(startup_stack->stack_top);

	vsyscall_entry = get_vsyscall_entry(STACK_INFO(p_envs));
//set_vsyscall_entry(STACK_INFO(p_envs),(unsigned long)&wrapped_syscall);

	set_vsyscall_entry(STACK_INFO(p_envs),(unsigned long)&wrapped_syscall_entry);
	VERBOSE(LOADER, "VDSO ENTRY:0x%x\n",(int)(int *)*STACK_AUXV_INFO(p_sysinfo));
	/* checks personality and re-exec */
	reexec();


	/* FIXME check platform */

	/* load real exec */
	void * entry_addr = NULL;
	if (load_exec(startup_stack, &entry_addr)) {
		/* load ld-linux.so.2 */
		DEBUG(LOADER, "need interp\n");
		load_real_interp(startup_stack, &entry_addr);
	} else {
		DEBUG(LOADER, "needn't interp\n");
	}

	/* setup return addr */
	SET_INTERP_RETADDR(startup_stack->stack_top, entry_addr);

	/* setup conf vals */
	read_env_conf();

	/* selection */
	return sampling();
}
// vim:ts=4:sw=4

