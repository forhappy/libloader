/* 
 * syscall.h
 * by WN @ Feb. 08, 2010
 */

#ifndef __SYSCALL_H
#define __SYSCALL_H
#include <stdint.h>
#include <asm/unistd.h>
#include <common/defs.h>
__BEGIN_DECLS

/* code from glibc 2.7 */
/* We need some help from the assembler to generate optimal code.  We
   define some macros here which later will be used.  */
asm (".L__X'%ebx = 1\n\t"
     ".L__X'%ecx = 2\n\t"
     ".L__X'%edx = 2\n\t"
     ".L__X'%eax = 3\n\t"
     ".L__X'%esi = 3\n\t"
     ".L__X'%edi = 3\n\t"
     ".L__X'%ebp = 3\n\t"
     ".L__X'%esp = 3\n\t"
     ".macro bpushl name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "error\n\t"
     ".else\n\t"
     "xchgl \\reg, %ebx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t"
     ".macro bpopl name reg\n\t"
     ".if 1 - \\name\n\t"
     ".if 2 - \\name\n\t"
     "error\n\t"
     ".else\n\t"
     "xchgl \\reg, %ebx\n\t"
     ".endif\n\t"
     ".endif\n\t"
     ".endm\n\t");

#define EXTRAVAR_0
#define EXTRAVAR_1
#define EXTRAVAR_2
#define EXTRAVAR_3
#define EXTRAVAR_4
#define EXTRAVAR_5 int ___xv;

#define LOADARGS_0
#define LOADARGS_1 \
    "bpushl .L__X'%k2, %k2\n\t"
#define LOADARGS_5 \
    "movl %%ebx, %3\n\t"						\
    "movl %2, %%ebx\n\t"
#define LOADARGS_2	LOADARGS_1
#define LOADARGS_3 \
    "xchgl %%ebx, %%edi\n\t"
#define LOADARGS_4	LOADARGS_3

#define RESTOREARGS_0
#define RESTOREARGS_1 \
    "bpopl .L__X'%k2, %k2\n\t"
#define RESTOREARGS_5 \
    "movl %3, %%ebx"
#define RESTOREARGS_2	RESTOREARGS_1
#define RESTOREARGS_3 \
    "xchgl %%edi, %%ebx\n\t"
#define RESTOREARGS_4	RESTOREARGS_3

#define ASMFMT_0()
#define ASMFMT_1(arg1) \
       , "cd" (arg1)
#define ASMFMT_2(arg1, arg2) \
       , "d" (arg1), "c" (arg2)
#define ASMFMT_3(arg1, arg2, arg3) \
       , "D" (arg1), "c" (arg2), "d" (arg3)
#define ASMFMT_4(arg1, arg2, arg3, arg4) \
       , "D" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
#define ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
	, "0" (arg1), "m" (_xv), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5)


#define INTERNAL_SYSCALL_0_5(name, enter_kernel, nr, args...) \
	({								\
	 	register int32_t ___retval;				\
	 	EXTRAVAR_##nr						\
		asm volatile (						\
			LOADARGS_##nr					\
			"movl %1, %%eax\n"				\
			enter_kernel					\
			RESTOREARGS_##nr				\
			: "=a" (___retval)					\
			: "i" (__NR_##name)		\
			  ASMFMT_##nr(args) : "memory", "cc");		\
	 	___retval;							\
	 })

#define __INTERNAL_SYSCALL_6(name, enter_kernel, arg1, arg2, arg3, arg4, arg5, arg6)	\
	     ({								\
	      	register int32_t ___retval;				\
		int32_t ___xv1, ___xv2;					\
		___xv2 = arg6;						\
		asm volatile (						\
			"movl %%ebx, %3\n"				\
			"movl %2, %%ebx\n"				\
			"movl %1, %%eax\n"				\
			"pushl %%ebp\n"					\
			"movl %8, %%ebp\n"				\
			enter_kernel					\
			"popl %%ebp\n"					\
			"movl %3, %%ebx\n"				\
			: "=a" (___retval)					\
			: "i" (__NR_##name),		\
			 "0" (arg1), "m" (___xv1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5), "m" (___xv2) \
			: "memory", "cc" 				\
			);						\
		___retval;							\
	      })

#define INTERNAL_SYSCALL(name, nr, args...)	\
	     INTERNAL_SYSCALL_##nr(name, args)

#define INTERNAL_SYSCALL_0(name, args...)	INTERNAL_SYSCALL_0_5(name, "call __vsyscall\n", 0)
#define INTERNAL_SYSCALL_1(name, args...)	INTERNAL_SYSCALL_0_5(name, "call __vsyscall\n", 1, args)
#define INTERNAL_SYSCALL_2(name, args...)	INTERNAL_SYSCALL_0_5(name, "call __vsyscall\n", 2, args)
#define INTERNAL_SYSCALL_3(name, args...)	INTERNAL_SYSCALL_0_5(name, "call __vsyscall\n", 3, args)
#define INTERNAL_SYSCALL_4(name, args...)	INTERNAL_SYSCALL_0_5(name, "call __vsyscall\n", 4, args)
#define INTERNAL_SYSCALL_5(name, args...)	INTERNAL_SYSCALL_0_5(name, "call __vsyscall\n", 5, args)
#define INTERNAL_SYSCALL_6(name, args...)	__INTERNAL_SYSCALL_6(name, "call __vsyscall\n", args)

#define INTERNAL_SYSCALL_int80(name, nr, args...)	\
	     INTERNAL_SYSCALL_int80_##nr(name, args)

#define INTERNAL_SYSCALL_int80_0(name, args...)	INTERNAL_SYSCALL_0_5(name, "int $0x80\n", 0, args)
#define INTERNAL_SYSCALL_int80_1(name, args...)	INTERNAL_SYSCALL_0_5(name, "int $0x80\n", 1, args)
#define INTERNAL_SYSCALL_int80_2(name, args...)	INTERNAL_SYSCALL_0_5(name, "int $0x80\n", 2, args)
#define INTERNAL_SYSCALL_int80_3(name, args...)	INTERNAL_SYSCALL_0_5(name, "int $0x80\n", 3, args)
#define INTERNAL_SYSCALL_int80_4(name, args...)	INTERNAL_SYSCALL_0_5(name, "int $0x80\n", 4, args)
#define INTERNAL_SYSCALL_int80_5(name, args...)	INTERNAL_SYSCALL_0_5(name, "int $0x80\n", 5, args)
#define INTERNAL_SYSCALL_int80_6(name, args...)	__INTERNAL_SYSCALL_6(name, "int $0x80\n", args)
#endif

__END_DECLS
// vim:ts=4:sw=4

