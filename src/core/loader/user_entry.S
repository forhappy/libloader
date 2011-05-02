/*
 * user_entry.S
 * by WN @ Nov. 08, 2010
 * modified by HP.FU @ Apr. 30, 2011
 */

#include <loader/asm_helper.S.h>

.text
.globl arch_user_entry_e
arch_user_entry_e:
	ENTER_ENTRY
	call arch_user_entry
	EXIT_TO(OFFSET_TPD_RESUME_ADDR)

/* don't generate executable stack */
.section        .note.GNU-stack,"",@progbits

// vim:ts=4:sw=4

