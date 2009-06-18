#include <stdint.h>
#include <asm/unistd.h>
#include "__ctype.h"

#define LOCAL __attribute__((visibility ("hidden")))
#define EXPORT __attribute__((visibility ("default")))


extern uint32_t vdso_ehdr;
extern uint32_t vdso_entrace;
const char stringXXX[] = "9876543210\n";
const char stringYYY[] = "1111111111\n";

const char help_string[] = "put help string here\n";


#define XXXXX(str)	asm volatile ( \
		"xchgl %%esi, %%ebx\n" \
		"int $0x80\n" \
		"xchgl %%ebx, %%esi\n" \
		:	\
		: "a" (4), "S" (1), "c" (str), "d" (sizeof(str)) \
		)

LOCAL void
show_help(void)
{
	asm volatile (
		"xchgl %%esi, %%ebx\n"
		"int $0x80\n"
		"xchgl %%ebx, %%esi\n"
		:
		: "a" (4), "S" (1), "c" (help_string), "d" (sizeof(help_string))
	);
}

uint32_t volatile intercept_start = 0x1234;

//extern LOCAL uint32_t intercept_start[];
LOCAL void
syscall_wrapper(void)
{
	XXXXX("syscall\n");
	if (intercept_start != 0x9876)
		return;
	XXXXX("intercept\n");
#if 0
		asm volatile (
				"xchgl %%esi, %%ebx\n"
				"int $0x80\n"
				"xchgl %%esi, %%ebx\n"
				:
				: "a" (4), "S" (1), "c" (stringYYY), "d" (11)
			     );
		asm volatile (
				"xchgl %%esi, %%ebx\n"
				"int $0x80\n"
				"xchgl %%esi, %%ebx\n"
				:
				: "a" (4), "S" (1), "c" (_ctype), "d" (11)
			     );
#endif
#if 1
	char string[256];
	int n;
	n = snprintf(string, 256, "eax=%d, %d\n", 12, 13);
		asm volatile (
				"xchgl %%esi, %%ebx\n"
				"int $0x80\n"
				"xchgl %%esi, %%ebx\n"
				:
				: "a" (4), "S" (1), "c" (string), "d" (n)
			     );

#endif

#if 0
	asm volatile (
		"xchgl %%esi, %%ebx\n"
		"int $0x80\n"
		"xchgl %%esi, %%ebx\n"
		:
		: "a" (4), "S" (1), "c" (string), "d" (n)
		);
#endif
#if 0
	while(1);

	asm volatile (
		"movl $1, %eax\n"
		"int $0x80\n");
#endif
}

LOCAL void
syscall_wrapper_init(void)
{
	asm volatile (
		"xchgl %%esi, %%ebx\n"
		"int $0x80\n"
		"xchgl %%esi, %%ebx\n"
		:
		: "a" (4), "S" (1), "c" (stringXXX), "d" (9)
		);
	return;
}

