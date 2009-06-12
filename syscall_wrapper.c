#include <stdint.h>
#include <asm/unistd.h>

#define LOCAL __attribute__((visibility ("hidden")))


#define asmlinkage_protect(n, ret, args...) \
	__asmlinkage_protect##n(ret, ##args)
#define __asmlinkage_protect_n(ret, args...) \
	__asm__ __volatile__ ("" : "=r" (ret) : "0" (ret), ##args)
#define __asmlinkage_protect0(ret) \
	__asmlinkage_protect_n(ret)
#define __asmlinkage_protect1(ret, arg1) \
	__asmlinkage_protect_n(ret, "g" (arg1))
#define __asmlinkage_protect2(ret, arg1, arg2) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2))
#define __asmlinkage_protect3(ret, arg1, arg2, arg3) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3))
#define __asmlinkage_protect4(ret, arg1, arg2, arg3, arg4) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3), \
			      "g" (arg4))
#define __asmlinkage_protect5(ret, arg1, arg2, arg3, arg4, arg5) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3), \
			      "g" (arg4), "g" (arg5))
#define __asmlinkage_protect6(ret, arg1, arg2, arg3, arg4, arg5, arg6) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3), \
			      "g" (arg4), "g" (arg5), "g" (arg6))


#define ASMFMT_0()
# define ASMFMT_1(arg1) \
	, "b" (arg1)
# define ASMFMT_2(arg1, arg2) \
	, "b" (arg1), "c" (arg2)
# define ASMFMT_3(arg1, arg2, arg3) \
	, "b" (arg1), "c" (arg2), "d" (arg3)
# define ASMFMT_4(arg1, arg2, arg3, arg4) \
	, "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
# define ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
	, "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5)





# define INTERNAL_SYSCALL(sysno, nr, args...) \
  ({									      \
    register unsigned int resultvar;					      \
    asm volatile (							      \
    "movl %1, %%eax\n\t"						      \
    "int $0x80\n\t"						      \
    : "=a" (resultvar)							      \
    : "i" (sysno) ASMFMT_##nr(args) : "memory", "cc");		      \
    (int) resultvar; })

# define INTERNAL_SYSCALL_NAME(name, nr, args...) \
	INTERNAL_SYSCALL(__NR_##name, nr, args)


LOCAL void
syscall_wrapper(void)
{
	char string[] = "test string\n";

	asm volatile ("pushl %eax;"
			"pushl %ebp;"
			"pushl %edi;"
			"pushl %esi;"
			"pushl %edx;"
			"pushl %ecx;"
			"pushl %ebx;"
			"pushl %eax;");

	asm volatile ("popl %eax;"
			"popl %ebx;"
			"popl %ecx;"
			"popl %edx;"
			"popl %esi;"
			"popl %edi;"
			"popl %ebp;"
			"popl %eax;");
//	INTERNAL_SYSCALL_NAME(write, 3, 1, string, sizeof(string));
//	INTERNAL_SYSCALL(sysno, 5, arg1, arg2, arg2, arg4, arg5);
}

