/* 
 * wrapper.c
 * by WN @ Jun. 16, 2009
 */
#define __KERNEL_STRICT_NAMES
typedef long long	__kernel_loff_t;
typedef __kernel_loff_t		loff_t;
#include <linux/elf.h>
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


/* map a so file into map_bias, return the symbol
 * */
uintptr_t
wrap_vdso(const char * so_filename,
		uintptr_t load_bias,
		uintptr_t old_ehdr,
		uintptr_t old_entry)
{
	void * image = load_file(so_filename);
	int nr = 0;
	struct elf_handler * h = elf_init(image, load_bias);
	struct elf32_phdr * phdr = elf_get_phdr_table(h, &nr);

	/* for each hdr */
	for (int i = 0; i< nr; i++, phdr ++) {
		SYS_FORCE("map phdr %d:\n", i);
		SYS_FORCE("type: %d\n", phdr->p_type);
	}

	free(image);
	return 0;
}

int main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		wrap_vdso("./syscall_wrapper_entrance.so",
				0x3000,
				0x1234,
				0x5678);
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
