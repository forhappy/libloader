#ifndef __CKPT_NEWPROCESS_H
#define __CKPT_NEWPROCESS_H

#include "injector.h"
#include "checkpoint/checkpoint.h"
#include "injector_utils.h"
#include "injector_debug.h"

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

