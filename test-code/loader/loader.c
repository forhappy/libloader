#include <stdio.h>

#include <asm/unistd.h>
#include <sys/personality.h>

#include "defs.h"
#include "injector.h"
#include "vsprintf.h"
#define BUFFER_SIZE     (16384)

#define assert(expr) \
	((expr) ? 0 : \
	 fdprintf(1, "assert failure at line %d\n", __LINE__))

#define printf(...) \
	fdprintf(1, __VA_ARGS__)

static char buffer[BUFFER_SIZE];

__attribute__((unused)) static int
printf_int80(const char * fmt, ...)
{

	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);

	INTERNAL_SYSCALL_int80(write, 3, 1, buffer, i);
	return i;
}



__attribute__((unused)) static int
fdprintf(int fd, const char * fmt, ...)
{

	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buffer, BUFFER_SIZE, fmt, args);
	va_end(args);

	INTERNAL_SYSCALL_int80(write, 3, fd, buffer, i);
	return i;
}

__attribute__((unused)) static int
vfdprintf(int fd, const char * fmt, va_list args)
{
	int i;
	i = vsnprintf(buffer, BUFFER_SIZE, fmt, args);

	INTERNAL_SYSCALL_int80(write, 3, fd, buffer, i);
	return i;
}


static void
exit(int status)
{
	asm volatile (
		"movl $1, %eax\n"
		"int $0x80\n"
	);
}

asm (
".globl _start\n\
 _start:\n\
 	push %esp\n\
	call xmain\n\
"
);


static void
reexecute(uintptr_t oldesp)
{
	/* read personality */
	int err = INTERNAL_SYSCALL_int80(personality, 1, 0xffffffff);
	assert(err >= 0);
	printf("personality = 0x%x\n", err);
	if (err & ADDR_NO_RANDOMIZE)
		return;
	printf("doesn't adopt fixed address, reexecve\n");
	int persona = err | ADDR_NO_RANDOMIZE;
	printf("persona should be 0x%x\n", persona);
	err = INTERNAL_SYSCALL_int80(personality, 1, persona);

	/* buildup env and cmdline */
	/* iterate over args */
	const char ** new_argv = NULL;
	const char ** new_env = NULL;
	uint32_t * ptr = (void*)oldesp;
	int argc = *ptr;
	printf("argc=%d\n", argc);
	ptr += 1;
	new_argv = (const char **)ptr;
	while (*(ptr) != 0) {
		printf("arg: %p: %s\n", ptr, (char*)(*ptr));
		ptr++;
	}

	/* env */
	ptr++;
	new_env = (const char **)ptr;
	while (*(ptr) != 0) {
		printf("env: %p: %s\n", ptr, (char*)(*ptr));
		ptr++;
	}

	/* execve */
	INTERNAL_SYSCALL_int80(execve, 3, new_argv[0], new_argv, new_env);
}

/* must be defined as static, or ld will create textrel object */
__attribute__((used, unused)) static int 
xmain(uintptr_t oldesp)
{

	printf("test vfdprintf: %p\n", xmain);
	printf("test vfdprintf: 0x%x\n", oldesp);

	/* check personality and reexecute */
	reexecute(oldesp);



	exit(0);
	return 0;
}

// vim:ts=4:sw=4

