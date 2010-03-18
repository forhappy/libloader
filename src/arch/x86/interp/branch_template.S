/*
 * branch_templates.S
 * by WN @ Mar. 18, 2010
 */

#include <asm_offsets.h>

#define DEF_Jxx_TEMPLATE(jxx)	\
.text;	\
.globl __##jxx##_template_start;	\
.globl __##jxx##_template_end;	\
.globl __##jxx##_template_taken_movl;	\
.globl __##jxx##_template_untaken_movl;	\
__##jxx##_template_start:	;	\
	movl %eax, %fs:OFFSET_TARGET;	\
	jxx 1f;				\
__##jxx##_template_untaken_movl:	;	\
	movl 0xffffffff, %eax;		\
	jmp 2f;				\
__##jxx##_template_taken_movl:			\
	1: movl 0xffffffff, %eax;	\
	2: xchgl %eax, %fs:OFFSET_TARGET;  \
	jmpl *%fs:OFFSET_LOGGER_LOG_BRANCH_TARGET;	\
__##jxx##_template_end: nop



DEF_Jxx_TEMPLATE(jo)
DEF_Jxx_TEMPLATE(jno)
DEF_Jxx_TEMPLATE(jb)
DEF_Jxx_TEMPLATE(jnb)
DEF_Jxx_TEMPLATE(jz)
DEF_Jxx_TEMPLATE(jnz)
DEF_Jxx_TEMPLATE(jna)
DEF_Jxx_TEMPLATE(ja)

