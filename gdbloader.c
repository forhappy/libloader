
#include "debug.h"
#include "exception.h"
#include "ckptutils.h"
#include "currf2_args.h"
#include "procutils.h"
#include "elfutils.h"
#include "ptraceutils.h"
#include "utils.h"

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

static struct opts * opts = NULL;
static struct ckpt_file * cf = NULL;
static pid_t child_pid = -1;

static void
inject_memory(void)
{
	struct mem_region * stack_r = NULL;

	SYS_TRACE("nr_regions=%d\n", cf->nr_regions);

	/* for each mem region in ckpt file */
	for (int i = 0; i < cf->nr_regions; i++) {
		struct mem_region * r = cf->regions[i];

		SYS_TRACE("range %d: 0x%x--0x%x (0x%x:0x%x): %s\n",
				i, r->start, r->end, r->prot, r->offset, r->fn);

		/* find the region already mapped */
		struct proc_entry e;
		bool_t res;
		uint32_t start, end;
		start = r->start;
		end = r->end;
		do {
			res = proc_find_in_range(&e, child_pid, start, end);

			if (res) {
				start = e.end;
				SYS_TRACE("\talready mapped in target: 0x%x--0x%x (0x%x:0x%x): %s\n",
						e.start, e.end, e.prot, e.offset, e.fn);

				/* check if 2 maps are same */
				if ((e.start != r->start) ||
						(e.end != r->end) ||
						(e.prot != r->prot) ||
						(strcmp(e.fn, r->fn) != 0))
				{
					/* different */
					/* special treat [heap] and [stack] */
					if (strcmp(e.fn, "[heap]") == 0) {
						/* heap size is different */
						/* that shouldn't happen, we restore heap address before
						 * calling this function*/
						THROW(EXCEPTION_FATAL, "heap address inconsistent");
					} else if (strcmp(e.fn, "[stack]") == 0) {
						if (strcmp(r->fn, "[stack]") != 0) {
							/* that shouldn't happen... */
							THROW(EXCEPTION_FATAL, "stack inconsistent");
						}
						/* stack can auto expand */
					} else if (strcmp(e.fn, "[vdso]") == 0) {
						THROW(EXCEPTION_FATAL, "vdso inconsistent");
					} else {
						/* unmap the already mapped file */
						/* first, check eip */
						struct user_regs_struct regs = ptrace_peekuser();
						if ((regs.eip >= e.start) && (regs.eip < e.end)) {
							/* target ckpt is special: it unmap itself... */
							/* currently we don't support it */
							THROW(EXCEPTION_FATAL, "eip (0x%x) inconsistent",
									(uint32_t)regs.eip);
						}

						/* now we can safely unmap that file or memregion */
						int err;
						SYS_TRACE("\tunmap 0x%x--0x%x\n", e.start, e.end);
						err = ptrace_syscall(munmap, 2, e.start, e.end - e.start);
						if (err < 0)
							THROW(EXCEPTION_FATAL, "unmap memrigon 0x%x--0x%x failed: %d",
									e.start, e.end, err);
					}
				}
			} else {
				start = r->end;
			}
		} while (start < end);

		/* now the region has been cleaned up, there may be 2 situations:
		 * 1. the desired region is empty;
		 * 2. the desired region is mapped, but the same as ckpt.
		 * here we use procutils to check it again. */
		res = proc_find_in_range(&e, child_pid, r->start, r->end);
		if (res) {
			SYS_TRACE("\tdesired region is mapped\n");
			if ((e.start != r->start) ||
					(e.end != r->end) ||
					(e.prot != r->prot) ||
					(strcmp(e.fn, r->fn) != 0))
				THROW(EXCEPTION_FATAL, "!@##@$!@");
		} else {
			SYS_TRACE("\tdesired region is unmapped\n");
			/* from the ckpt, find the file and do the map */
			uint32_t map_addr = r->start;
			uint32_t size = r->end - r->start;
			uint32_t prot = r->prot;
			uint32_t flags = MAP_FIXED | MAP_EXECUTABLE | MAP_PRIVATE;
			if ((r->fn_len <= 1) || (r->fn[0] == '\0')) {
				/* this is not a file map */
				uint32_t ret_addr;
				flags |= MAP_ANONYMOUS;
				SYS_TRACE("\tdo the anonymouse map\n");
				ret_addr = ptrace_syscall(mmap2, 6,
						map_addr, size, prot,
						flags, 0, 0);
				CTHROW(map_addr == ret_addr, "mmap2 failed, return 0x%x, not 0x%x",
						ret_addr, map_addr);
			} else {
				/* this is a file map */
				uint32_t fn_pos;
				/* push the filename */
				fn_pos = ptrace_push(r->fn, strlen(r->fn) + 1, TRUE);

				int fd = ptrace_syscall(open, 3, fn_pos, O_RDONLY, 0);
				CTHROW(fd >= 0, "open file %s failed: %d", r->fn, fd);

				SYS_TRACE("\tdo the map\n");
				uint32_t off = r->offset;
				uint32_t ret_addr = ptrace_syscall(mmap2, 6,
						map_addr, size, prot,
						flags, fd, off >> PAGE_SHIFT);
				CTHROW(ret_addr == map_addr, "mmap2 file %s failed: return 0x%x",
						r->fn, ret_addr);

				ptrace_syscall(close, 1, fd);
			}
		}

		/* now the memory region has been built up, we then poke
		 * memory into it */
		/* don't update stack here. although in most case the stack
		 * is the last region we meet, there are some special situations.
		 * for example in compat memlayout. the stack may be polluted by pervious
		 * ptrace_push operation. */
		if (strcmp(r->fn, "[stack]") == 0)
			stack_r = r;
		else
			ptrace_updmem(r->f_pos + cf->ckpt_img,
					r->start,
					r->end - r->start);
	}

	CTHROW(stack_r != NULL, "no \"[stack]\" found");
	/* we poke stack at last */
	ptrace_updmem(stack_r->f_pos + cf->ckpt_img,
			stack_r->start,
			stack_r->end - stack_r->start);
}

static void
fix_libpthread(uint32_t * sym_stack_used, uint32_t * sym_stack_user)
{
	int err;
	struct proc_entry pe;
	memset(&pe, 0, sizeof(pe));
	strncpy(pe.fn, opts->pthread_so_fn, 256);
	pe.bits = PE_FILE;
	err = proc_fill_entry(&pe, child_pid);
	
	/* if err != 0, then there is no libpthread. */
	if (err != 0) {
		SYS_TRACE("no %s found, needn't fix libpthread\n", opts->pthread_so_fn);
		return;
	}

	SYS_TRACE("find %s mapped ");

	/* find symbol __stack_user and stack_used */
	void * img = load_file(pe.fn);
	struct elf_handler * lp_so = elf_init(img, pe.start);

	*sym_stack_user = elf_get_symbol_address(lp_so, "__stack_user");
	*sym_stack_used = elf_get_symbol_address(lp_so, "stack_used");

	elf_cleanup(lp_so);
	free(img);
}

static void
gdbloader_main(const char * target_fn)
{
	/* check: target_fn should be same as argv[0] */
	if (strcmp(target_fn, cf->cmdline[0]) != 0) {
		SYS_FATAL("target should be %s, not %s\n", cf->cmdline[0], target_fn);
		THROW(EXCEPTION_FATAL, "cmdline error");
	}

	/* execve child */
	child_pid = ptrace_execve(target_fn, cf->cmdline, cf->environ);

	/* inject memory */
	/* before we inject memory, we need to restore heap */
	uint32_t heap_end;
	heap_end = ptrace_syscall(brk, 1, cf->state->brk);
	CTHROW(heap_end == cf->state->brk, "restore heap failed: %d",
			heap_end);
	SYS_TRACE("restore heap to 0x%x\n", heap_end);

	inject_memory();

	/* then, we retrive the inject so file, enter from
	 * __debug_entry. we need to push:
	 * nothing.
	 * process can retrive all from state vector.
	 * and we cannot use stack now
	 * */
	/* NOTICE: the state_vector should be saved in the ckpt memory,
	 * we needn't restore them in ptrace process. let the inject so
	 * to do it. */

	/* from the opts get the so-file bias */
	uint32_t inj_bias = opts->inj_bias;
	/* use procutils to get the file */
	struct proc_entry e;
	e.start = inj_bias;
	e.bits = PE_START; 
	proc_fill_entry(&e, child_pid);
	SYS_TRACE("inject so is %s\n", e.fn);

	/* use elfutils to retrive the symbol */
	void * img = load_file(e.fn);
	struct elf_handler * inj_so = elf_init(img, inj_bias);

	uintptr_t debug_entry = elf_get_symbol_address(inj_so,
			opts->entry);
	SYS_TRACE("symbol %s at 0x%x\n", opts->entry, debug_entry);

	elf_cleanup(inj_so);
	free(img);

	/* we have to restore register here... */
	SYS_FORCE("pid=%d\n", child_pid);
	SYS_FORCE("eip=0x%x\n", cf->state->regs.eip);
	ptrace_pokeuser(cf->state->regs);
	SYS_TRACE("eax=0x%x\n", cf->state->regs.eax);
	SYS_TRACE("ebx=0x%x\n", cf->state->regs.ebx);
	SYS_TRACE("ecx=0x%x\n", cf->state->regs.ecx);
	SYS_TRACE("edx=0x%x\n", cf->state->regs.edx);
	SYS_TRACE("esi=0x%x\n", cf->state->regs.esi);
	SYS_TRACE("edi=0x%x\n", cf->state->regs.edi);
	SYS_TRACE("ebp=0x%x\n", cf->state->regs.ebp);
	SYS_TRACE("esp=0x%x\n", cf->state->regs.esp);
	SYS_TRACE("gs=0x%x\n", cf->state->regs.gs);
	SYS_TRACE("es=0x%x\n", cf->state->regs.es);

	/* we push eip at the top of the new stack */
	ptrace_push(&cf->state->regs.eip, sizeof(uint32_t), FALSE);

	/* fix libpthread problem:
	 *
	 * when gdb attaches to target, if it find libpthread, gdb
	 * will try to use libthread_db to retrive thread-local info.
	 * some data, like `errno', is TLS and need those info.
	 *
	 * When gdb do the work, it use ptrace to peek memory from target image.
	 * so gdb will see the original thread info, the tid is different from
	 * current pid, therefore gdb will think there are at least 2 threads and
	 * then it will try to attach to the 'old' one and definitely fail. When this
	 * failure occure, gdb print a warning message.
	 *
	 * We have 2 ways to solve this problem:
	 *
	 * 1. add a syscall into kernel's code, change its pid. it is simple.
	 * 2. change the image when gdb attach.
	 *
	 * We choose the 2nd one because we prefer use space solution.
	 *
	 * */
	uint32_t sym_stack_used = 0, sym_stack_user = 0;
	if (opts->fix_pthread_tid) {
		fix_libpthread(&sym_stack_used, &sym_stack_user);
		SYS_WARNING("sym_stack_used=0x%x, sym_stack_user=0x%x\n",
				sym_stack_used, sym_stack_user);
	}

	/* we push those 2 addresses onto the stack */
	ptrace_push(&sym_stack_used, sizeof(uint32_t), FALSE);
	ptrace_push(&sym_stack_user, sizeof(uint32_t), FALSE);

	/* move eip and detach, let the target process to run */
	ptrace_goto(debug_entry);

	/* detach in main */
	return;
}
	

int
main(int argc, char * argv[])
{
	char * target_fn = NULL;
	DEBUG_INIT(NULL);
	opts = parse_args(argc, argv);
	assert(opts != NULL);

	target_fn = argv[opts->cmd_idx];
	assert(opts->cmd_idx < argc);

	/* check ckpt file and target file */
	struct stat s;
	if (stat(opts->ckpt_fn, &s) != 0) {
		SYS_ERROR("file %s doesn't exists\n", opts->ckpt_fn);
		return -1;
	}

	if (!S_ISREG(s.st_mode)) {
		SYS_ERROR("file %s not a regular file\n", opts->ckpt_fn);
		return -1;
	}

	if (stat(target_fn, &s) != 0) {
		SYS_ERROR("file %s doesn't exists\n", target_fn);
		return -1;
	}

	if (!S_ISREG(s.st_mode)) {
		SYS_ERROR("file %s not a regular file\n", target_fn);
		return -1;
	}


	/* start */
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		cf = load_ckpt_file(opts->ckpt_fn);
		gdbloader_main(target_fn);
		close_ckpt_file(cf);
		ptrace_detach(TRUE);
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			break;
		default:
			if (child_pid != -1)
				ptrace_kill();
			print_exception(FATAL, SYSTEM, exp);
			break;
	}
	do_cleanup();
	return 0;
}
// vim:ts=4:sw=4

