/*
 * compiler.S
 * by WN @ Mar. 16, 2010
 */

#include <asm_offsets.h>

.text

.globl check_logger_buffer
check_logger_buffer:
	jmpl *%fs:OFFSET_LOGGER_CHECK_BUFFER_RETURN
	nop

.globl syscall_entry
syscall_entry:
	nop

// vim:ts=4:sw=4

