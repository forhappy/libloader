#include <stdio.h>

#include <asm/unistd.h>
#include <sys/personality.h>

#include <fcntl.h>
#include <unistd.h>

#include <elf.h>
#include <alloca.h>
#include <sys/mman.h>

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
//		printf("arg: %p: %s\n", ptr, (char*)(*ptr));
		ptr++;
	}

	/* env */
	ptr++;
	new_env = (const char **)ptr;
	while (*(ptr) != 0) {
//		printf("env: %p: %s\n", ptr, (char*)(*ptr));
		ptr++;
	}

	/* execve */
	INTERNAL_SYSCALL_int80(execve, 3, new_argv[0], new_argv, new_env);
}

static void
load_ld(uintptr_t oldesp)
{
	int err;
	/* open and mmap */
	int fd = INTERNAL_SYSCALL_int80(open, 2,
			"/lib/ld-linux.so.2", O_RDONLY);
	assert(fd >= 0);

	Elf32_Ehdr ehdr;
	/* load the elf head */
	err = INTERNAL_SYSCALL_int80(read, 3, fd, &ehdr, sizeof(ehdr));
	assert(err == sizeof(ehdr));

	assert(memcmp(ehdr.e_ident, ELFMAG, SELFMAG) == 0);
	printf("check ld-linux.so.2's header, magic OK\n");

	/* ********************************************* */
	assert(ehdr.e_type == ET_DYN);
	assert(ehdr.e_machine == EM_386);
	uintptr_t entry = ehdr.e_entry;
	int nr_pht = ehdr.e_phnum;
	assert(ehdr.e_phentsize == sizeof(Elf32_Phdr));

	/* load program headers */
	err = INTERNAL_SYSCALL_int80(lseek, 3, fd,
			ehdr.e_phoff, SEEK_SET);
	assert(err >= 0);

	Elf32_Phdr * phdrs = alloca(sizeof(Elf32_Phdr) * nr_pht);
	assert(phdrs != NULL);

	err = INTERNAL_SYSCALL_int80(read, 3, fd, phdrs, sizeof(Elf32_Phdr) * nr_pht);
	assert(err == sizeof(Elf32_Phdr) * nr_pht);
	printf("load phdr table OK\n");

	/* iterate over its phtable */
	/* compute total load size */
	uint32_t total_sz = 0;
	{
		uint32_t s, e;
		s = 0xffffffffUL;
		e = 0;
		for (int i = 0; i < nr_pht; i++) {
			Elf32_Phdr * p = phdrs + i;
			if (p->p_type != PT_LOAD)
				continue;
			if (s > p->p_vaddr)
				s = p->p_vaddr;
			if (e < p->p_vaddr + p->p_memsz)
				e = p->p_vaddr + p->p_memsz;
			assert(p->p_align == 0x1000);
		}
		total_sz = e - s;
		printf("s=0x%x, e=0x%x, total_sz=0x%x\n",
				s, e, total_sz);
		/* roundup */
		total_sz = (total_sz + (0x1000-1)) & (0xffffe000);
	}

	void * map_addr;
	/* map the big sz then unmap */
	map_addr = INTERNAL_SYSCALL_int80(mmap2, 6,
			NULL, total_sz, PROT_READ,
			MAP_PRIVATE, fd, 0);

	assert(map_addr < 0xC0000000UL);
	assert(!(((uint32_t)map_addr) & 0x1FFFUL));

	printf("map_addr=0x%x\n", map_addr);

#if 0
	/* unmap it! */
	err = INTERNAL_SYSCALL_int80(munmap, 2,
			map_addr, total_sz);
	assert(err == 0);


	/* begin mapping */
	for (int i = 0; i < nr_pht; i++) {
		Elf32_Phdr * p = phdrs + i;
		if (p->p_type != PT_LOAD)
			continue;
		uintptr_t addr = p->p_vaddr;
		uintptr_t off = p->p_offset;
		uint32_t size = p->p_memsz;
	}
#endif
	/* ********************************************* */


	INTERNAL_SYSCALL_int80(close, 1, fd);
}


/* must be defined as static, or ld will create textrel object */
__attribute__((used, unused)) static int 
xmain(uintptr_t oldesp)
{

	printf("test vfdprintf: %p\n", xmain);
	printf("test vfdprintf: 0x%x\n", oldesp);

	/* check personality and reexecute */
	reexecute(oldesp);

	/* try to load /lib/ld-linux.so.2, and transfer control to it */
	load_ld(oldesp);


	exit(0);
	return 0;
}

// vim:ts=4:sw=4

