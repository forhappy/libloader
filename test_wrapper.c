/* 
 * test_wrapper.c
 * by WN @ Jun. 15, 2009
 */
//#define __KERNEL_STRICT_NAMES
//typedef long long	__kernel_loff_t;
//typedef __kernel_loff_t		loff_t;
//#include <linux/elf.h>
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

static void
map_wrap_so(const char * so_file, uintptr_t load_bias,
		uint32_t * pvdso_entrance,
		uint32_t * pvdso_ehdr)
{
	uint32_t name_pos;
	int fd, err;

	struct stat s;
	err = stat(so_file, &s);
	assert_errno_throw("stat file %s failed", so_file);
	assert_throw(S_ISREG(s.st_mode), "file %s not a regular file", so_file);
	/* don't use off_t, it may not be a 32 bit word! */
	int32_t fsize = s.st_size;
	SYS_TRACE("desired so file length is %d\n", fsize);

	/* elf operations */
	void * so_image = load_file(so_file);
	struct elf_handler * h = elf_init(so_image, load_bias);
	/* load program headers */
	int nr_phdr = 0;
	struct elf32_phdr * phdr = elf_get_phdr_table(h, &nr_phdr);
	assert_throw(((phdr != NULL) && (nr_phdr != 0)),
			"load phdr of file %s failed\n", so_file);
	/* find the entry symbol */
	uintptr_t entry_addr = elf_get_symbol_address(h, "syscall_wrapper_entrace");
	SYS_TRACE("wrapper func address will be 0x%x\n", entry_addr);


	name_pos = ptrace_push(so_file, strlen(so_file), TRUE);
	fd = ptrace_syscall(open, 3, name_pos, O_RDONLY, 0);
	assert_throw(fd >= 0, "open sofile for child failed, return %d", fd);
	SYS_TRACE("open so file for child, fd=%d\n", fd);


	/* for each program header */
	for (int i = 0; i < nr_phdr; i++, phdr ++) {
		SYS_FORCE("phdr %d, type=%d, flag=0x%x\n", i,
				phdr->p_type, phdr->p_flags);
		if (phdr->p_type != PT_LOAD)
			continue;

		int elf_prot = 0, elf_flags = 0;
		
		if (phdr->p_flags & PF_R)
			elf_prot |= PROT_READ;
		if (phdr->p_flags & PF_W)
			elf_prot |= PROT_WRITE;
		if (phdr->p_flags & PF_X)
			elf_prot |= PROT_EXEC;

		elf_flags = MAP_PRIVATE | MAP_EXECUTABLE;

		unsigned long size = phdr->p_filesz + ELF_PAGEOFFSET(phdr->p_vaddr);
		unsigned long off = phdr->p_offset - ELF_PAGEOFFSET(phdr->p_vaddr);
		int32_t map_addr = load_bias + phdr->p_vaddr - ELF_PAGEOFFSET(phdr->p_vaddr);

		map_addr = ptrace_syscall(mmap2, 6,
				map_addr, size,
				elf_prot, elf_flags | MAP_FIXED,
				fd, off);
		assert_throw(map_addr != 0xffffffff, "map wrap so failed, return 0x%x", map_addr);
	}
	elf_cleanup(h);
	free(so_image);

	if (pvdso_ehdr)
		*pvdso_ehdr = load_bias;
	if (pvdso_entrance)
		*pvdso_entrance = entry_addr;
}

static void
wrapper_main(int argc, char * argv[])
{
	pid_t pid;
	
	/* first, trace target */
	pid = ptrace_execve("./target", argv);

	/* dump stack, find sysinfo */
	struct user_regs_struct regs;
	regs = ptrace_peekuser();

	SYS_TRACE("stack top=0x%x\n", regs.esp);

	struct proc_entry stack_entry;
	stack_entry.addr = regs.esp;
	stack_entry.bits = PE_ADDR;
	proc_fill_entry(&stack_entry, pid);
	SYS_TRACE("stack range: 0x%x-0x%x\n", stack_entry.start, stack_entry.end);

	/* dump the stack and find SYSINFO */
	uint32_t stack_size = stack_entry.end - regs.esp;
	void * stack_image = malloc(stack_size);
	void * elf_table;
	assert(stack_image != 0);
	ptrace_dupmem(stack_image, regs.esp, stack_size);
	elf_table = get_elf_table(stack_image);
	assert(elf_table != NULL);
	SYS_TRACE("elf_table start at %p\n", elf_table);

	/* iterator elf table */
	uint32_t * auxv_entry = elf_table;
	uint32_t * pvdso_ehdr = NULL, * pvdso_entrance = NULL;
	while (*auxv_entry != 0) {
		if (auxv_entry[0] == AT_SYSINFO)
			pvdso_entrance = &auxv_entry[1];
		if (auxv_entry[0] == AT_SYSINFO_EHDR)
			pvdso_ehdr = &auxv_entry[1];
		auxv_entry += 2;
	}
	assert_throw(pvdso_ehdr != NULL, "cannot find vdso ehdr");
	assert_throw(pvdso_entrance != NULL, "cannot find vdso entrance");
	SYS_FORCE("vdso mapped at 0x%x, entry=0x%x\n",
			*pvdso_ehdr, *pvdso_entrance);

	/* begin to map wrapper so */
	map_wrap_so("./syscall_wrapper_entrance.so", 0x3000,
			pvdso_entrance, pvdso_ehdr);

	/* reset the stack */

	ptrace_updmem(stack_image, regs.esp, stack_size);
	free(stack_image);
	ptrace_detach(TRUE);
}

int main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		wrapper_main(argc, argv);
	} CATCH(exp) {
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

