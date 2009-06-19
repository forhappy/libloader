/* 
 * currf2.c
 * by WN @ Jun. 19, 2009
 */

#include <elf.h>
#include <linux/user.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"
#include "procutils.h"
#include "utils.h"
#include "elfutils.h"
#include "currf2_args.h"

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


#define ETHROW(x...)	assert_errno_throw(x)
#define CTHROW(c, x...) assert_throw((c), x)

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
static struct elf_file inject = {NULL, NULL, NULL};
static void * stack_image = NULL;
static uint32_t * pvdso_entry = NULL;
static uint32_t * pvdso_ehdr = NULL;
static uint32_t old_vdso_entry = 0;
static uint32_t old_vdso_ehdr = 0;

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
	if (inject.h != NULL) {
		elf_cleanup(inject.h);
		inject.h = NULL;
	}
	if (inject.m != NULL) {
		free(inject.m);
		inject.m = NULL;
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
	old_vdso_ehdr = *pvdso_ehdr;
	old_vdso_entry = *pvdso_entry;
}

static void
map_injector(void)
{
	int err;
	int nr;
	struct elf32_phdr * phdr = elf_get_phdr_table(inject.h, &nr);
	CTHROW((phdr != NULL) && (nr != 0),
			"load phdr of injector file %s failed", inject.fn);

	/* open the file */
	uint32_t fn_pos = ptrace_push(inject.fn, strlen(inject.fn), TRUE);
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
		elf_prot = PROT_WRITE;
		elf_flags = MAP_PRIVATE | MAP_EXECUTABLE;

		unsigned long size = phdr->p_filesz + ELF_PAGEOFFSET(phdr->p_vaddr);
		unsigned long off = phdr->p_offset - ELF_PAGEOFFSET(phdr->p_vaddr);
		int32_t map_addr = opts->inj_bias + phdr->p_vaddr - ELF_PAGEOFFSET(phdr->p_vaddr);

		SYS_TRACE("map to 0x%x, size=0x%x, off=0x%x\n", map_addr, size, off);

		int32_t ret_addr = ptrace_syscall(mmap2, 6,
				map_addr, size, elf_prot,
				elf_flags | MAP_FIXED, fd, off >> PAGE_SHIFT);
		CTHROW(map_addr == ret_addr, "map injector failed, return 0x%x", ret_addr);
	}

	/* close the file */
	err = ptrace_syscall(close, 1, fd);
	if (err != 0)
		WARNING(SYSTEM, "close injector in child code failed: %d\n", err);
}

static void
currf2_main(int argc, char * argv[])
{
	/* check files */
	check_file(target.fn);
	check_file(inject.fn);
	SYS_TRACE("target exec: %s\n",
			target.fn);
	SYS_TRACE("inject so file: %s\n",
			inject.fn);
	
	target.m = load_file(target.fn);
	target.h = elf_init(target.m, 0);

	inject.m = load_file(inject.fn);
	inject.h = elf_init(inject.m, opts->inj_bias);

	child_pid = ptrace_execve(target.fn, argv + opts->cmd_idx);

	/* dump stack */
	struct user_regs_struct r;
	struct proc_entry spe;

	r = ptrace_peekuser();
	spe.addr = r.esp;
	spe.bits = PE_ADDR;
	proc_fill_entry(&spe, child_pid);
	SYS_TRACE("stack range: 0x%x-0x%x\n", spe.start, spe.end);

	size_t stk_sz = spe.end - r.esp;
	stack_image = malloc(stk_sz);
	assert(stack_image != NULL);
	ptrace_dupmem(stack_image, r.esp, stk_sz);

	find_vdso();
	map_injector();

	/* set the stack pointers */
	*pvdso_ehdr = opts->inj_bias;
	*pvdso_entry = elf_get_symbol_address(inject.h,
			opts->wrap_sym);

	/* update stack */
	ptrace_updmem(stack_image, r.esp, stk_sz);
	free(stack_image);
	stack_image = NULL;

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
	inject.fn = opts->inj_so;

	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		make_cleanup(&main_clup_s);
		currf2_main(argc, argv);
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

