/* 
 * test_wrapper.c
 * by WN @ Jun. 15, 2009
 */

#include <linux/user.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "debug.h"
#include "exception.h"
#include "ptraceutils.h"
#include "procutils.h"
#include "utils.h"
#include "elfutils.h"

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
map_wrap_so(uint32_t * pvdso_entrance,
		uint32_t * pvdso_ehdr)
{
	static char so_file[] = "./syscall_wrapper_entrance.so";
	uint32_t name_pos;
	int fd, err;

	name_pos = ptrace_push(so_file, sizeof(so_file), TRUE);
	fd = ptrace_syscall(open, 3, name_pos, O_RDONLY, 0);
	assert_throw(fd >= 0, "open sofile for child failed, return %d", fd);
	SYS_TRACE("open so file for child, fd=%d\n", fd);

	struct stat s;
	err = stat(so_file, &s);
	assert_errno_throw("stat file %s failed", so_file);
	assert_throw(S_ISREG(s.st_mode), "file %s not a regular file", so_file);
	/* don't use off_t, it may not be a 32 bit word! */
	int32_t fsize = s.st_size;
	SYS_TRACE("desired so file length is %d\n", fsize);

	/* find the entry symbol */
	void * so_image = load_file(so_file);
	struct elf_handler * h = elf_init(so_image, 0x3000);
	uintptr_t entry_addr = elf_get_symbol_address(h, "syscall_wrapper_entrace");
	SYS_TRACE("wrapper func address will be 0x%x\n", entry_addr);
	elf_cleanup(h);
	free(so_image);

	int32_t map_addr;
	map_addr = ptrace_syscall(mmap2, 6,
			0x3000, fsize,
			PROT_READ|PROT_WRITE|PROT_EXEC,
			MAP_PRIVATE|MAP_FIXED,
			fd, 0);
	assert_throw(map_addr == 0x3000, "map wrap so failed, return %d", map_addr);

	if (pvdso_ehdr)
		*pvdso_ehdr = map_addr;
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
	map_wrap_so(pvdso_entrance, pvdso_ehdr);

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

