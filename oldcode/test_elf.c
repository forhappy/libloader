#include <elfutils.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "debug.h"
#include "exception.h"
#include "elfutils.h"

static uint8_t * image = NULL;
static FILE * image_fp = NULL;

static void
main_clup(struct cleanup * __clup)
{
	if (image != NULL) {
		free(image);
		image = NULL;
	}

	if (image_fp != NULL) {
		fclose(image_fp);
		image_fp = NULL;
	}
}

static struct cleanup clup = {
	.function = main_clup,
};



int main(int argc, char * argv[])
{
	DEBUG_INIT(NULL);
	VERBOSE(SYSTEM, "System start!\n");
	FORCE(SYSTEM, "Force message\n");

	if (argc < 2)
		return 0;

	VERBOSE(SYSTEM, "open file %s\n", argv[1]);
	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		int err;
		struct stat _s;
		err = stat(argv[1], &_s);
		if (err != 0)
			THROW(EXCEPTION_FATAL, strerror(errno));
		if (!S_ISREG(_s.st_mode))
			THROW(EXCEPTION_FATAL, "Not a regular file");

		TRACE(SYSTEM, "file length: %d\n", _s.st_size);
		make_cleanup(&clup);
		image = malloc(_s.st_size);
		assert(image != NULL);

		FILE * image_fp = fopen(argv[1], "rb");
		if (image_fp == NULL)
			THROW(EXCEPTION_FATAL, strerror(errno));

		err = fread(image, _s.st_size, 1, image_fp);
		if (err != 1)
			THROW(EXCEPTION_FATAL, strerror(errno));
		fclose(image_fp);
		image_fp = NULL;


		struct elf_handler * handler = elf_init(image, 0);
		uintptr_t main_addr;
		main_addr = elf_get_symbol_address(handler, "main");
		VERBOSE(SYSTEM, "Address of main: 0x%x\n", main_addr);

		elf_cleanup(handler);

	}
	CATCH(exp) {
		case EXCEPTION_NO_ERROR:
			TRACE(SYSTEM, "Finished successfully\n");
			break;
		default:
			print_exception(FATAL, SYSTEM, exp);
	}

	do_cleanup();
	show_mem_info();
	return 0;
}
// vim:ts=4:sw=4

