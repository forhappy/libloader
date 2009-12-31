#include <stdio.h>

#include <asm/unistd.h>
#include "defs.h"
#include "injector.h"
#include "vsprintf.h"
#define BUFFER_SIZE     (16384)

static char buffer[BUFFER_SIZE];

static int
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



static int
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

static int
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
	call xmain\n\
"
);


/* must be defined as static, or ld will create textrel object */
__attribute__((used, unused)) static int 
xmain()
{

	fdprintf(1, "test vfdprintf: %p\n", xmain);
	/*  */
//	printf("aaa\n");
//	printf("aaa %d\n", 20);

	exit(0);
	return 0;
}

