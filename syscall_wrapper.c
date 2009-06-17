#include <stdint.h>
#include <asm/unistd.h>

#define LOCAL __attribute__((visibility ("hidden")))
#define EXPORT __attribute__((visibility ("default")))


extern uint32_t vdso_ehdr;
extern uint32_t vdso_entrace;
const char stringXXX[] = "9876543210\n";
const char stringYYY[] = "1111111111\n";

const char help_string[] = "put help string here\n";

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


LOCAL void
syscall_wrapper(void)
{
	asm volatile (
		"xchgl %%esi, %%ebx\n"
		"int $0x80\n"
		"xchgl %%esi, %%ebx\n"
		:
		: "a" (4), "S" (1), "c" (stringXXX), "d" (11)
		);

	asm volatile (
		"xchgl %%esi, %%ebx\n"
		"int $0x80\n"
		"xchgl %%esi, %%ebx\n"
		:
		: "a" (4), "S" (1), "c" (stringXXX), "d" (11)
		);


	while(1);

	asm volatile (
		"movl $1, %eax\n"
		"int $0x80\n");
	return;
}

void
syscall_wrapper_init(void)
{
	asm volatile (
		"movl $0x4, %eax\n"
		"movl $1, %ebx\n"
		"movl stringXXX, %ecx\n"
		"movl $9, %edx\n"
		"int $0x80\n"
		);
	return;
}

