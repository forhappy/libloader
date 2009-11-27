/* 
 * snitchaser.c
 * by WN @ Jun. 19, 2009
 */

#include <elf.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"
#include "procutils.h"
#include "utils.h"
#include "elfutils.h"
#include "snitchaser_args.h"
#include "checkpoint/checkpoint.h"


/* don't include kernel's file */
struct elf32_phdr{
  Elf32_Word	p_type;
  Elf32_Off	p_offset;
  Elf32_Addr	p_vaddr;
  Elf32_Addr	p_paddr;
  Elf32_Word	p_filesz;
  Elf32_Word	p_memsz;
  Elf32_Word	p_flags;
  Elf32_Word	p_align;
};

/* copy from kernel's code (binfmt_elf.c) */
#ifndef PAGE_SIZE
#define PAGE_SIZE (0x1000)
#endif
#define ELF_MIN_ALIGN	PAGE_SIZE
#define ELF_PAGESTART(_v) ((_v) & ~(unsigned long)(ELF_MIN_ALIGN-1))
#define ELF_PAGEOFFSET(_v) ((_v) & (ELF_MIN_ALIGN-1))
#define ELF_PAGEALIGN(_v) (((_v) + ELF_MIN_ALIGN - 1) & ~(ELF_MIN_ALIGN - 1))

static void *
get_elf_table(void * stack_image)
{
	uint32_t * p = stack_image;
	/* argc */
	p++;
	/* args */
	while (*p != 0)
		p++;
	p += 1;
	/* envs */
	while (*p != 0)
		p++;
	p += 1;
	return p;
}

static struct opts * opts = NULL;
static pid_t child_pid = 0;

struct elf_file {
	void * m;
	struct elf_handler * h;
	char * fn;
};

static struct elf_file target = {NULL, NULL, NULL};
static struct elf_file injector = {NULL, NULL, NULL};
static void * stack_image = NULL;
static uint32_t * pvdso_entry = NULL;
static uint32_t * pvdso_ehdr = NULL;
static uint32_t old_vdso_entry = 0;

/* below 2 vars are for sigaction and rt_sigaction */
uint32_t wrapped_sigreturn = 0;
uint32_t wrapped_rt_sigreturn = 0;
uint32_t wrapped_sighandler = 0;

static void main_clup(struct cleanup * cleanup)
{
	if (target.h != NULL) {
		elf_cleanup(target.h);
		target.h = NULL;
	}
	if (target.m != NULL) {
		free(target.m);
		target.m = NULL;
	}
	if (injector.h != NULL) {
		elf_cleanup(injector.h);
		injector.h = NULL;
	}
	if (injector.m != NULL) {
		free(injector.m);
		injector.m = NULL;
	}
	if (stack_image != NULL) {
		free(stack_image);
		stack_image = NULL;
	}
}

struct cleanup main_clup_s = {
	.function = main_clup,
};


static void
check_file(char * fn)
{
	struct stat s;
	stat(fn, &s);
	ETHROW("stat file %s failed", fn);
	CTHROW(S_ISREG(s.st_mode), "file %s not regular file", fn);
	return;
}

static int
syscall_hook(struct user_regs_struct u, bool_t before)
{
	struct syscall_regs regs;
#define SETREG(x)	regs.x = u.x
	SETREG(orig_eax);
	SETREG(eax);
	SETREG(ebx);
	SETREG(ecx);
	SETREG(edx);
	SETREG(esi);
	SETREG(edi);
	SETREG(ebp);
	SETREG(eip);
	SETREG(esp);
#undef SETREG
	if (before)
		return before_syscall(&regs);
	else
		return after_syscall(&regs);
}

static void
find_vdso(void)
{
	void * tbl = get_elf_table(stack_image);
	assert(tbl != NULL);
	SYS_TRACE("elf table start at 0x%x\n", tbl);

	uint32_t * entry = tbl;
	while (*entry != 0) {
		if (entry[0] == AT_SYSINFO)
			pvdso_entry = &entry[1];
		if (entry[0] == AT_SYSINFO_EHDR)
			pvdso_ehdr = &entry[1];
		entry += 2;
	}
	CTHROW(pvdso_entry != NULL, "cannot find vdso entry");
	CTHROW(pvdso_ehdr != NULL, "cannot find vdso ehdr");
	SYS_VERBOSE("vdso mapped at 0x%x, entry=0x%x\n",
			*pvdso_ehdr, *pvdso_entry);
	old_vdso_entry = *pvdso_entry;
}

static void
map_injector(void)
{
	int err;
	int nr;
	struct elf32_phdr * phdr = elf_get_phdr_table(injector.h, &nr);
	CTHROW((phdr != NULL) && (nr != 0),
			"load phdr of injector file %s failed", injector.fn);

	/* open the file */
	/* +1 for the last '\0' */
	uint32_t fn_pos = ptrace_push(injector.fn, strlen(injector.fn) + 1, TRUE);
	int fd = ptrace_syscall(open, 3, fn_pos, O_RDONLY, 0);
	CTHROW(fd >= 0, "open injector from client code failed, return %d", fd);
	SYS_TRACE("open injector for child, fd=%d\n", fd);

	/* for each program header */
	for (int i = 0; i < nr; i++, phdr++) {
		SYS_TRACE("phdr %d, type=0x%x, flag=0x%x\n",
				i, phdr->p_type, phdr->p_flags);
		if (phdr->p_type != PT_LOAD)
			continue;

		int elf_prot = 0, elf_flags = 0;
		if (phdr->p_flags & PF_R)
			elf_prot |= PROT_READ;
		if (phdr->p_flags & PF_X)
			elf_prot |= PROT_EXEC;
		elf_prot |= PROT_WRITE;
		elf_flags = MAP_PRIVATE | MAP_EXECUTABLE;

		unsigned long size = phdr->p_filesz + ELF_PAGEOFFSET(phdr->p_vaddr);
		unsigned long off = phdr->p_offset - ELF_PAGEOFFSET(phdr->p_vaddr);
		int32_t map_addr = opts->inj_bias + phdr->p_vaddr - ELF_PAGEOFFSET(phdr->p_vaddr);

		SYS_TRACE("map to 0x%x, size=0x%x, memsz=0x%x, off=0x%x\n", map_addr, size, phdr->p_memsz, off);

		uint32_t ret_addr = ptrace_syscall(mmap2, 6,
				map_addr, size, elf_prot,
				elf_flags | MAP_FIXED, fd, off >> PAGE_SHIFT);
		CTHROW(map_addr == ret_addr, "map injector failed, return 0x%x", ret_addr);

		if (phdr->p_memsz > size) {
			/* we need to map additional 0 pages */
			uint32_t add_sz = ELF_PAGEALIGN(phdr->p_memsz - ELF_PAGEALIGN(size));
			uint32_t start = ELF_PAGEALIGN(map_addr + size);

			SYS_TRACE("meed additional zeroed page: memsz=0x%x, mapped size=0x%x, additional size=0x%x\n",
					phdr->p_memsz, size, add_sz);

			uint32_t ret_addr = ptrace_syscall(mmap2, 6,
					start, add_sz, elf_prot,
					elf_flags | MAP_FIXED | MAP_ANONYMOUS, 0, 0);
			CTHROW(ret_addr == start,
					"map injector additional page failed, return %x", ret_addr);

			/* zero those pages */
			int zsz = phdr->p_memsz - size;
			void * zeros = calloc(1, zsz);
			ptrace_updmem(zeros, map_addr + size, zsz);
			free(zeros);
		}
	}

	/* close the file */
	err = ptrace_syscall(close, 1, fd);
	if (err != 0)
		WARNING(SYSTEM, "close injector in child code failed: %d\n", err);
}

static void
reloc_injector(uintptr_t addr, const char * sym, int type, int sym_val)
{

	SYS_TRACE("relocate symbol '%s' at address 0x%x\n", sym, addr);

	if ((type != R_386_PC32) &&
			(type != R_386_32) &&
			(type != R_386_RELATIVE) &&
			(type != R_386_GLOB_DAT)) {
		SYS_WARNING("doesn't support reloc type 0x%x", type);
		return;
	}
	
	uint32_t real_val;

	if (sym[0] == '\0') {
		if (type != R_386_RELATIVE) {
			SYS_WARNING("don't know how to relocate this type: 0x%x\n",
					type);
			return;
		}
		real_val = 0;
	} else {
		if (strncmp(sym, opts->old_vsyscall, strlen(opts->old_vsyscall)) == 0) {
			real_val = old_vdso_entry;
		} else if (type != R_386_GLOB_DAT) {
			SYS_WARNING("don't know how to relocate this symbol: '%s'\n",
					sym);
			return;
		}
	}


	switch (type) {
		case R_386_32: {
			break;
		}
		case R_386_PC32: {
			int32_t old_val;
			ptrace_dupmem(&old_val, addr, sizeof(old_val));
			real_val = real_val + old_val - addr;
			break;
		}
		case R_386_RELATIVE: {
			int32_t old_val;
			ptrace_dupmem(&old_val, addr, sizeof(old_val));
			real_val = old_val + opts->inj_bias;
			break;
		}
		case R_386_GLOB_DAT: {
			real_val = sym_val + opts->inj_bias;
			break;
		}
	}

	ptrace_updmem(&real_val, addr, sizeof(real_val));
	SYS_TRACE("\tset to 0x%x\n", real_val);
	return;
}

static void
inject_statvec(void)
{
	uintptr_t addr = elf_get_symbol_address(injector.h, opts->state_vect);
	CTHROW(addr != 0, "wrong symbol: %s", opts->state_vect);
	ptrace_updmem(&state_vector, addr, sizeof(state_vector));
}

static void
inject_injopts(void)
{
	uintptr_t addr = elf_get_symbol_address(injector.h, opts->injector_opts);
	CTHROW(addr != 0, "wrong symbol: %s", opts->injector_opts);

	injector_opts.logger_threshold = opts->logger_threshold;
	injector_opts.trace_fork = opts->trace_fork;
	injector_opts.untraced = opts->untraced;

	ptrace_updmem(&injector_opts, addr, sizeof(injector_opts));
	
}

static void
snitchaser_main(int argc, char * argv[])
{
	int err;
	/* check files */
	check_file(target.fn);
	check_file(injector.fn);
	SYS_TRACE("target exec: %s\n",
			target.fn);
	SYS_TRACE("inject so file: %s\n",
			injector.fn);
	
	target.m = load_file(target.fn);
	target.h = elf_init(target.m, 0);

	injector.m = load_file(injector.fn);
	injector.h = elf_init(injector.m, opts->inj_bias);

	/* execve child */
	child_pid = ptrace_execve(target.fn, argv + opts->cmd_idx, environ);

	/* dump stack */
	struct user_regs_struct r;
	struct proc_entry spe;

	r = ptrace_peekuser();
	spe.addr = r.esp;
	spe.bits = PE_ADDR;

	err = proc_fill_entry(&spe, child_pid);
	CTHROW(err == 0, "addr 0x%x is not mapped\n", (uint32_t)r.esp);
	SYS_TRACE("stack range: 0x%x-0x%x\n", spe.start, spe.end);

	size_t stk_sz = spe.end - r.esp;
	stack_image = malloc(stk_sz);
	assert(stack_image != NULL);
	ptrace_dupmem(stack_image, r.esp, stk_sz);

	find_vdso();
	map_injector();
	/* find sigreturn and rt_sigreturn */
	wrapped_sigreturn = elf_get_symbol_address(injector.h,
			opts->sigreturn);
	CTHROW(wrapped_sigreturn != 0, "symbol \"%s\" not found",
			opts->sigreturn);
	wrapped_rt_sigreturn = elf_get_symbol_address(injector.h,
			opts->rt_sigreturn);
	CTHROW(wrapped_rt_sigreturn != 0, "symbol \"%s\" not found",
			opts->rt_sigreturn);
	wrapped_sighandler = elf_get_symbol_address(injector.h,
			opts->sighandler);
	CTHROW(wrapped_sighandler != 0, "symbol \"%s\" not found",
			opts->sighandler);


	/* relocate injector */
	SYS_TRACE("begin to relocate symbol %s\n", opts->old_vsyscall);
	elf_reloc_symbols(injector.h, reloc_injector);

	/* set the stack pointers */
	*pvdso_ehdr = opts->inj_bias;
	*pvdso_entry = elf_get_symbol_address(injector.h,
			opts->wrap_sym);
	CTHROW(*pvdso_entry != 0, "wrong symbol: %s", opts->wrap_sym);

	uintptr_t injector_entry = elf_get_symbol_address(injector.h,
			opts->entry);
	CTHROW(injector_entry != 0, "wrong symbol: %s",
			opts->entry);

	/* update stack */
	ptrace_updmem(stack_image, r.esp, stk_sz);
	free(stack_image);
	stack_image = NULL;

	err = checkpoint_init();
	CTHROW(err == 0, "chkp init failed: %s", strerror(errno));

	err = logger_init(child_pid);
	CTHROW(err == 0, "logger init failed: %s", strerror(errno));

	/* insert a breakpoint at main */
	uintptr_t main_entry = elf_get_symbol_address(target.h, "main");
	CTHROW(main_entry != 0, "wrong symbol: main");

	SYS_TRACE("insert breakpoint at 0x%x\n", main_entry);
	ptrace_insert_bkpt(main_entry);

	/* set the wrapper entry code to 'int $80; ret' */
#define TRIVAL_WRAPPER	"\xcd\x80\xc3"
#define SZ_TRIVAL_WRAPPER	sizeof(TRIVAL_WRAPPER)
	uint8_t wrapper_bkup[SZ_TRIVAL_WRAPPER];
	ptrace_dupmem(wrapper_bkup, *pvdso_entry, SZ_TRIVAL_WRAPPER);
	ptrace_updmem(TRIVAL_WRAPPER, *pvdso_entry, SZ_TRIVAL_WRAPPER);


	uintptr_t cur_eip = 0;
	do {
		cur_eip = ptrace_next_syscall(syscall_hook);
	} while (cur_eip != main_entry + 1);
	SYS_VERBOSE("come to user code\n");


	err = logger_close();
	CTHROW(err == 0, "logger close failed: %s\n", strerror(errno));

	ptrace_resume();

	/* reset the wrapper entry */
	ptrace_updmem(wrapper_bkup, *pvdso_entry, SZ_TRIVAL_WRAPPER);

	/* inject the 'state vector' */
	inject_statvec();

	/* set and inject the options of injector */
	inject_injopts();

	/* goto the injector '__entry' */

	/* order is important, this is ABI between snitchaser and injector */
	/* put the ret val of __entry. it is also the 2nd arg */
	ptrace_push(&main_entry, sizeof(uint32_t), FALSE);

	/* 1st arg: old sysinfo addr */
	ptrace_push(&old_vdso_entry, sizeof(uint32_t), FALSE);

	ptrace_goto(injector_entry);

	ptrace_detach(TRUE);

	return;
}

int
main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	opts = parse_args(argc, argv);
	assert(opts != NULL);

	target.fn = argv[opts->cmd_idx];
	injector.fn = opts->inj_so;

	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		make_cleanup(&main_clup_s);
		snitchaser_main(argc, argv);
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			break;
		default:
			ptrace_kill();
			print_exception(FATAL, SYSTEM, exp);
			break;
	}
	do_cleanup();
	return 0;
}


// vim:ts=4:sw=4

