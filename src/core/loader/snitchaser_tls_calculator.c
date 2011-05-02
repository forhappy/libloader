/* 
 * tls_offset_helper.c
 * by WN @ Nov. 05, 2010
 * modified by HP.Fu @ Apr 29, 2011
 */

#include <loader/tls.h>
#include <loader/snitchaser_tpd.h>
#include <loader/processor.h>

#define __off_t_defined
#include <stdio.h>

#define xPRINT(p, element, name)	\
	do {						\
		printf("#define OFFSET_" #p "_" #name "\t(0x%x)\n",\
				tls_offset(element));\
	} while(0)

#define PRINTTD(el, name) xPRINT(TD, td.el, name)
#define PRINTTPD(el, name) xPRINT(TPD, tpd.el, name)


static void
xcompute(void)
{
	printf("#define OFFSET_PUSHA_REGS (0x%x)\n",
			TLS_AREA_SIZE -
			sizeof(struct ___bottom_tls) - 
			sizeof(struct pusha_regs));
	printf("#define OFFSET_TLS_HEAD (0x%x)\n",
			TLS_AREA_SIZE - sizeof(struct ___bottom_tls));

	PRINTTD(fs_val, FS_VAL);
	PRINTTD(nr, NR);
	PRINTTD(list, LIST);
	PRINTTD(start_addr, START_ADDR);

	printf("#define OFFSET_TPD\t(0x%x)\n", tls_offset(tpd));

	PRINTTPD(old_stack_top, OLD_STACK_TOP);
#ifdef STACK_TOP
# undef STACK_TOP
#endif
	PRINTTPD(stack_top, STACK_TOP);
	PRINTTPD(pid, PID);
	PRINTTPD(tid, TID);
	PRINTTPD(bb, BB);
	PRINTTPD(bb.buffer[0], BB_START_ADDR);
	PRINTTPD(bb.buffer[BIGBUFFER_SZ], BB_END_ADDR);
	PRINTTPD(resume_addr, RESUME_ADDR);
	PRINTTPD(logger_stub.asm_curr_ptr, LOGGER_CURR_PTR);
	PRINTTPD(logger_stub.asm_end_ptr, LOGGER_END_PTR);
	PRINTTPD(logger_stub, LOGGER_STUB);
	PRINTTPD(mm_stub, MM_STUB);

}

int main(int argc, char * argv[])
{
	printf("/* this is generated file, don't edit it! */\n");
	printf("#ifndef __SNITCHASER_ASM_OFFSET\n");
	printf("#define __SNITCHASER_ASM_OFFSET\n");
	printf("/* sizeof(struct thread_private_data): %d */\n",
			sizeof(struct thread_private_data));

	xcompute();

	printf("#endif\n");
	printf("\n// vim:ts=4:sw=4\n");
	return 0;
}

// vim:ts=4:sw=4

