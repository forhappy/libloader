
#include "debug.h"
#include "exception.h"
#include "ckptutils.h"
#include "currf2_args.h"
#include "procutils.h"
#include "ptraceutils.h"

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
		ptrace_updmem(r->f_pos + cf->ckpt_img,
				r->start,
				r->end - r->start);
	}
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
	child_pid = ptrace_execve(target_fn, cf->cmdline);

	/* inject memory */
	/* before we inject memory, we need to restore heap */
	uint32_t heap_end;
	heap_end = ptrace_syscall(brk, 1, cf->state->brk);
	CTHROW(heap_end == cf->state->brk, "restore heap failed: %d",
			heap_end);
	SYS_TRACE("restore heap to 0x%x\n", heap_end);
	inject_memory();

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
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			if (child_pid != -1)
				ptrace_kill();
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

