/* 
 * asm/debug.c
 */

#include <asm/syscall.h>
#include <asm/vsprintf.h>
#include <asm/utils.h>
#include <sys/mman.h>

static char fdprintf_buf[1024];

int
vfdprintf(int fd, const char * fmt, va_list args)
{
	int n = vsnprintf(fdprintf_buf, 1024, fmt, args);
	INTERNAL_SYSCALL_int80(write, 3, fd, fdprintf_buf, n);
	return n;
}

int
fdprintf(int fd, const char * fmt, ...)
{
	int n;
	va_list ap;
	va_start(ap, fmt);
	n = vfdprintf(fd, fmt, ap);
	va_end(ap);
	return n;
}

int
__printf(const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int n = vfdprintf(2, fmt, ap);
	va_end(ap);
	return n;
	
}

void *
alloc_pages(int nr, bool_t exec)
{
	void * p = (void*)INTERNAL_SYSCALL_int80(mmap2, 6,
			0,
			nr * PAGE_SIZE,
			PROT_READ | PROT_WRITE | (exec ? PROT_EXEC : 0),
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0);
	assert((uint32_t)p < 0xc0000000);
	return p;
}

void
free_pages(void * start, int nr)
{
	int err;
	start = (void*)(((uint32_t)start) & 0xfffff000);
	err = INTERNAL_SYSCALL_int80(munmap, 2,
			start, nr * PAGE_SIZE);
	assert(err == 0);
	return;
}

// vim:ts=4:sw=4

