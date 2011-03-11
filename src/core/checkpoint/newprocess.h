#ifndef __CKPT_NEWPROCESS_H
#define __CKPT_NEWPROCESS_H

#include "../libwrapper/injector.h"
#include "checkpoint.h"
#include "../libwrapper/injector_utils.h"
#include "../libwrapper/injector_debug.h"

extern SCOPE void
do_child_fork(unsigned long clone_flags,
		unsigned long stack_start,
		const struct syscall_regs * regs,
		uintptr_t parent_tidptr,
		uintptr_t child_tidptr);

extern SCOPE void
do_parent_fork(unsigned long clone_flags,
		unsigned long stack_start,
		const struct syscall_regs * regs,
		uintptr_t parent_tidptr,
		uintptr_t child_tidptr);

#endif

