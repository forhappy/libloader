
#include "checkpoint/checkpoint.h"
#include "checkpoint/syscall_table.h"
#include "libwrapper/signal_defs.h"

#include "exception.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <asm/unistd.h>


static FILE * log_fp = NULL;

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

void
seek_logger(int off, int origin)
{
	int err;
	err = fseek(log_fp, off, origin);
	if (err != 0)
		THROW(EXCEPTION_RESOURCE_LOST, "seek failed");
}


int finished = 0;
static void
printer_main(void)
{
	finished = 0;
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

			int16_t sigflag;
			read_logger(&sigflag, sizeof(sigflag));
			if (sigflag != -1) {
				printf("process distrubed by signal %d\n", -sigflag-1);
				/* skip: */
				int32_t frame_sz = read_int32();
				if ((frame_sz != sizeof(struct sigframe)) &&
						(frame_sz != sizeof(struct rt_sigframe)))
				{
					THROW(EXCEPTION_FATAL, "frame_sz %d, strange...", frame_sz);
				}
				/* skip frame */
				skip(frame_sz);
				/* skip 2 regs */
				skip(sizeof(struct syscall_regs));
				skip(sizeof(struct syscall_regs));
				/* skip rt_sigaction */
				skip(sizeof(struct k_sigaction));
			} else {
				syscall_table[nr].output_handler(nr);
			}
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

