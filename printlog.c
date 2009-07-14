
#include "checkpoint/checkpoint.h"
#include "checkpoint/syscall_table.h"

#include "exception.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

FILE * log_fp = NULL;

void
read_logger(void * buffer, int sz)
{
	int err;
	if (buffer == NULL) {
		err = fseek(log_fp, sz, SEEK_CUR);
		if (err != 0)
			THROW(EXCEPTION_RESOURCE_LOST, "fseek failed");
		return;
	}

	err = fread(buffer, 1, sz, log_fp);
	if (err != sz)
		THROW(EXCEPTION_RESOURCE_LOST, "read failed");
}

static void
printer_main(void)
{

	int finished = 0;

	while (!finished) {
		uint32_t nr = 0;

		volatile struct exception exp;
		TRY_CATCH(exp, MASK_RESOURCE_LOST) {
			read_logger(&nr, sizeof(nr));

			SYS_TRACE("syscall nr %u\n", nr);
			if (nr > NR_SYSCALLS)
				THROW(EXCEPTION_FATAL, "no handler for syscall %u", nr);

			if (syscall_table[nr].output_handler == NULL)
				THROW(EXCEPTION_FATAL, "no handler for syscall %u", nr);
			syscall_table[nr].output_handler();

		} CATCH (exp) {
			case EXCEPTION_NO_ERROR:
				break;
			case EXCEPTION_RESOURCE_LOST:
				SYS_VERBOSE("log file over\n");
				finished = 1;
				break;
			default:
				RETHROW(exp);
		}

	}
}

int
main(int argc, char * argv[])
{

	DEBUG_INIT(NULL);

	log_fp = fopen(argv[1], "rb");
	if (log_fp == 0) {
		SYS_WARNING("open file %s failed: %s\n",
				argv[1], strerror(errno));
		return -1;
	}

	volatile struct exception exp;
	TRY_CATCH(exp, MASK_ALL) {
		printer_main();
		fclose(log_fp);
		log_fp = NULL;
	} CATCH (exp) {
		case EXCEPTION_NO_ERROR:
			SYS_VERBOSE("successfully finish\n");
			break;
		default:
			if (log_fp != NULL) {
				fclose(log_fp);
				log_fp = NULL;
			}
			print_exception(FATAL, SYSTEM, exp);
			break;
	}

	do_cleanup();
	return 0;
}

// vim:ts=4:sw=4
