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

#define DEFAULT_LOADER	"/lib/ld-linux.so.2"
//#define DEFAULT_LOADER	"/lib/ld-linux.so.2"


#define AT_NULL   0	/* end of vector */
#define AT_IGNORE 1	/* entry should be ignored */
#define AT_EXECFD 2	/* file descriptor of program */
#define AT_PHDR   3	/* program headers for program */
#define AT_PHENT  4	/* size of program header entry */
#define AT_PHNUM  5	/* number of program headers */
#define AT_PAGESZ 6	/* system page size */
#define AT_BASE   7	/* base address of interpreter */
#define AT_FLAGS  8	/* flags */
#define AT_ENTRY  9	/* entry point of program */
#define AT_NOTELF 10	/* program is not ELF */
#define AT_UID    11	/* real uid */
#define AT_EUID   12	/* effective uid */
#define AT_GID    13	/* real gid */
#define AT_EGID   14	/* effective gid */
#define AT_PLATFORM 15  /* string identifying CPU for optimizations */
#define AT_HWCAP  16    /* arch dependent hints at CPU capabilities */
#define AT_CLKTCK 17	/* frequency at which times() increments */
/* AT_* values 18 through 22 are reserved */
#define AT_SECURE 23   /* secure mode boolean */
#define AT_BASE_PLATFORM 24	/* string identifying real platform, may
				 * differ from AT_PLATFORM. */
#define AT_RANDOM 25	/* address of 16 random bytes */

#define AT_EXECFN  31	/* filename of program */








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
 	push $0x2234\n\
 	push %esp\n\
	call xmain\n\
	pop %eax\n\
	ret\n\
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
	printf("new_argv[0]=%s\n", new_argv[0]);
	err = INTERNAL_SYSCALL_int80(execve, 3, new_argv[0], new_argv, new_env);
}

static void * interp_base = NULL;
static Elf32_Phdr * aux_phdr = NULL;
static int  aux_nr_phdr = 0;

static void
fix_aux(void * esp)
{
	printf("fix aux: 0x%x\n", esp);
	uint32_t * ptr = (void*)esp;
	while (*ptr != 0)
		ptr ++;
	ptr ++;
	while (*ptr != 0)
		ptr++;
	ptr ++;
	/* begin vector */
	int i = 0;
	while (*ptr != AT_NULL) {
		printf("aux %d: %d, 0x%x\n", i, ptr[0], ptr[1]);

		if (ptr[0] == AT_BASE) {
			assert(interp_base != NULL);
			ptr[1] = (uint32_t)interp_base;
		}

		if (ptr[0] == AT_PHDR)
			aux_phdr = (Elf32_Phdr *)ptr[1];
		if (ptr[0] == AT_PHENT)
			assert(ptr[1] == sizeof(*aux_phdr));
		if (ptr[0] == AT_PHNUM)
			aux_nr_phdr = ptr[1];

		ptr += 2;
		i ++;
	}

	/* iterate over phdr */
	for (int i = 0; i < aux_nr_phdr; i++) {
		if (aux_phdr[i].p_type == PT_INTERP) {
			/* change interp */
			uintptr_t start, end;
			
			start = (uintptr_t)aux_phdr[i].p_vaddr;
			end = start + aux_phdr[i].p_memsz;

			start = start & 0xfffff000UL;
			end = (end + 0xfff) & 0xfffff000UL;

			INTERNAL_SYSCALL_int80(mprotect, 3, start, end - start, PROT_READ|PROT_WRITE|PROT_EXEC);
			strcpy((void*)aux_phdr[i].p_vaddr,
					DEFAULT_LOADER);
		}
	}

}


static uintptr_t
load_ld(uintptr_t oldesp)
{
	int err;
	/* open and mmap */
	int fd = INTERNAL_SYSCALL_int80(open, 2,
			DEFAULT_LOADER, O_RDONLY);
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
#if 0
		/* roundup */
		total_sz = (total_sz + (0x1000-1)) & (0xfffff000);
#endif
	}

	void * map_addr = 0;
	/* map the big sz then unmap */
#if 0
	map_addr = INTERNAL_SYSCALL_int80(mmap2, 6,
			NULL, total_sz, PROT_READ,
			MAP_PRIVATE, fd, 0);

	assert(map_addr < 0xC0000000UL);
	assert(!(((uint32_t)map_addr) & 0x1FFFUL));

	printf("map_addr=0x%x\n", map_addr);
#endif

	/* map the full image */
	bool_t first_map = TRUE;
	for (int i = 0; i < nr_pht; i++) {

		Elf32_Phdr * p = phdrs + i;
		if (p->p_type != PT_LOAD)
			continue;
		uint32_t elf_type = MAP_PRIVATE | MAP_DENYWRITE;
		uint32_t elf_prot = 0;

		if (p->p_flags & PF_R)
			elf_prot = PROT_READ;
		if (p->p_flags & PF_W)
			elf_prot |= PROT_WRITE;
		if (p->p_flags & PF_X)
			elf_prot |= PROT_EXEC;

		if (first_map) {
			/* FIXME! */
			assert(p->p_offset == 0);
			map_addr = (void*)INTERNAL_SYSCALL_int80(mmap2, 6,
					NULL, total_sz, elf_prot, elf_type,
					fd, 0);
			assert((uintptr_t)map_addr < 0xc0000000UL);
			printf("map address: 0x%x, total_sz=0x%x, map_end=0x%x\n", map_addr,
					total_sz, (uintptr_t)map_addr + total_sz);
			first_map = FALSE;
			interp_base = map_addr;

			uintptr_t unmap_start = (uintptr_t)map_addr + (uintptr_t)p->p_memsz;
			unmap_start = (unmap_start + 0xfff) & 0xfffff000;
			uintptr_t unmap_end = (uintptr_t)map_addr + total_sz;
			unmap_end = (unmap_end + 0xfff) & 0xfffff000;

			printf("unmap from 0x%x to 0x%x\n",
					unmap_start, unmap_end);
			INTERNAL_SYSCALL_int80(munmap, 2,
					unmap_start,
					unmap_end - unmap_start);
		} else {
			uint32_t start, end;
			start = p->p_vaddr + (uintptr_t)map_addr;
			end = start + p->p_filesz;
			start = start & 0xfffff000UL;
			end = (end + 0x0fff) & 0xfffff000UL;
			/* map */
			err = INTERNAL_SYSCALL_int80(mmap2, 6,
					start, end - start,
					elf_prot, elf_type | MAP_FIXED,
					fd, p->p_offset >> 12);
			printf("map address: 0x%x, length=0x%x\n", err,
					end - start);

#if 1
			start = p->p_vaddr + (uintptr_t)map_addr;
			if (p->p_memsz > p->p_filesz) {
				/* pad zero and map new pages */
				/* whether we need new page? */
				uintptr_t fend, mend;
				fend = start + p->p_filesz;
				mend = start + p->p_memsz;
				if ((fend & 0xfffff000) != (mend & 0xfffff000)) {
					/* we need new page */
					uintptr_t start = (fend + 0xfff) & 0xfffff000;
					uintptr_t end = (mend + 0xfff) & 0xfffff000;
					assert(start != end);
					err = INTERNAL_SYSCALL_int80(mmap2, 6,
							start, end - start,
							elf_prot, elf_type | MAP_FIXED | MAP_ANONYMOUS,
							fd, 0);
					assert(err == start);
					printf("extern padding: new pages start from 0x%x\n", err);
				}
				printf("start=0x%x, fend=0x%x, mend=0x%x\n",
						start, fend, mend);
				printf("padding zero from 0x%x, len=%x\n", fend,
						((mend + 0xfff) & 0xfffff000) - fend);
				memset((void*)fend, 0, ((mend + 0xfff) & 0xfffff000) - fend);
			}
#endif
		}
	}

	/* ********************************************* */


	INTERNAL_SYSCALL_int80(close, 1, fd);
	return (uintptr_t)(entry + map_addr);
}


/* must be defined as static, or ld will create textrel object */
__attribute__((used, unused)) static int 
xmain(uintptr_t oldesp, volatile uintptr_t retcode)
{

	oldesp += 4;
	printf("test vfdprintf: %p\n", xmain);
	printf("test vfdprintf: 0x%x\n", oldesp);
	printf("test vfdprintf: 0x%x\n", retcode);

	/* check personality and reexecute */
	reexecute(oldesp);

	/* try to load /lib/ld-linux.so.2, and transfer control to it */
	uintptr_t entry = load_ld(oldesp);
	printf("entry=0x%x\n", entry);


	/* fix auxvector */
	fix_aux((void*)oldesp);

//	while(1);
//	exit(0);
	retcode = entry;
	return 0;
}

// vim:ts=4:sw=4

