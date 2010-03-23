/* 
 * x86/asm/logger.h
 * by WN @ Mar. 16, 2010
 */

#ifndef __X86_ASM_LOGGER_H
#define __X86_ASM_LOGGER_H

#include <config.h>
#include <common/defs.h>
extern ATTR_HIDDEN void
check_logger_buffer(void);

extern ATTR_HIDDEN void
syscall_entry(void);

/* we need 8 additional bytes because of rdtsc */
#define LOGGER_ADDITIONAL_BYTES	(8)

#endif

