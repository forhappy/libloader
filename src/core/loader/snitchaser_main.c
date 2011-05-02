#include <config.h>
#include <defs.h>
#include <debug.h>
#include <syscall.h>
#include <linux/kernel.h>
#include <linux/personality.h>
#include <loader/elf.h>
#include <loader/startup_stack.h>
#include <loader/env_conf.h>
#include <loader/processor.h>
#include <loader/proc.h>
#include <loader/tls.h>
#include <loader/snitchaser_main.h>
#include <loader/bigbuffer.h>
#include <loader/snitchaser_tpd.h>
#include <loader/user_entry.h>
#include <loader/mm.h>
#define R_386_NONE	   0		/* No reloc */
#define R_386_32	   1		/* Direct 32 bit  */
#define R_386_PC32	   2		/* PC relative 32 bit */
#define R_386_GOT32	   3		/* 32 bit GOT entry */
#define R_386_PLT32	   4		/* 32 bit PLT address */
#define R_386_COPY	   5		/* Copy symbol at runtime */
#define R_386_GLOB_DAT	   6		/* Create GOT entry */
#define R_386_JMP_SLOT	   7		/* Create PLT entry */
#define R_386_RELATIVE	   8		/* Adjust by program base */
#define R_386_GOTOFF	   9		/* 32 bit offset to GOT */
#define R_386_GOTPC	   10		/* 32 bit PC relative offset to GOT */
#define R_386_32PLT	   11
#define R_386_TLS_TPOFF	   14		/* Offset in static TLS block */
#define R_386_TLS_IE	   15		/* Address of GOT entry for static TLS
					   block offset */
#define R_386_TLS_GOTIE	   16		/* GOT entry for static TLS block
					   offset */
#define R_386_TLS_LE	   17		/* Offset relative to static TLS
					   block */
#define R_386_TLS_GD	   18		/* Direct 32 bit for GNU version of
					   general dynamic thread local data */
#define R_386_TLS_LDM	   19		/* Direct 32 bit for GNU version of
					   local dynamic thread local data
					   in LE code */
#define R_386_16	   20
#define R_386_PC16	   21
#define R_386_8		   22
#define R_386_PC8	   23
#define R_386_TLS_GD_32	   24		/* Direct 32 bit for general dynamic
					   thread local data */
#define R_386_TLS_GD_PUSH  25		/* Tag for pushl in GD TLS code */
#define R_386_TLS_GD_CALL  26		/* Relocation for call to
					   __tls_get_addr() */
#define R_386_TLS_GD_POP   27		/* Tag for popl in GD TLS code */
#define R_386_TLS_LDM_32   28		/* Direct 32 bit for local dynamic
					   thread local data in LE code */
#define R_386_TLS_LDM_PUSH 29		/* Tag for pushl in LDM TLS code */
#define R_386_TLS_LDM_CALL 30		/* Relocation for call to
					   __tls_get_addr() in LDM code */
#define R_386_TLS_LDM_POP  31		/* Tag for popl in LDM TLS code */
#define R_386_TLS_LDO_32   32		/* Offset relative to TLS block */
#define R_386_TLS_IE_32	   33		/* GOT entry for negated static TLS
					   block offset */
#define R_386_TLS_LE_32	   34		/* Negated offset relative to static
					   TLS block */
#define R_386_TLS_DTPMOD32 35		/* ID of module containing symbol */
#define R_386_TLS_DTPOFF32 36		/* Offset in TLS block */
#define R_386_TLS_TPOFF32  37		/* Negated offset in static TLS block */
/* 38? */
#define R_386_TLS_GOTDESC  39		/* GOT offset for TLS descriptor.  */
#define R_386_TLS_DESC_CALL 40		/* Marker of call through TLS
					   descriptor for
					   relaxation.  */
#define R_386_TLS_DESC     41		/* TLS descriptor containing
					   pointer to code and to
					   argument, returning the TLS
					   offset for the symbol.  */
#define R_386_IRELATIVE	   42		/* Adjust indirectly by program base */
/* Keep this the last entry.  */
#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#define O_CREAT		   0100	/* not fcntl */
#define O_EXCL		   0200	/* not fcntl */
#define O_NOCTTY	   0400	/* not fcntl */
#define O_TRUNC		  01000	/* not fcntl */
#define O_APPEND	  02000
#define O_NONBLOCK	  04000
#define O_NDELAY	O_NONBLOCK
#define O_SYNC	       04010000
#define O_FSYNC		 O_SYNC
#define O_ASYNC		 020000

#ifdef __ASSEMBLY__
#define _AC(X,Y)	X
#define _AT(T,X)	X
#else
#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)
#define _AT(T,X)	((T)(X))
#endif

#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define ELF_EXEC_PAGESIZE	4096

#if ELF_EXEC_PAGESIZE > PAGE_SIZE
#define ELF_MIN_ALIGN	ELF_EXEC_PAGESIZE
#else
#define ELF_MIN_ALIGN	PAGE_SIZE
#endif

#ifndef ELF_CORE_EFLAGS
#define ELF_CORE_EFLAGS	0
#endif
#define ELF_PAGESTART(_v) ((_v) & ~(unsigned long)(ELF_MIN_ALIGN-1))
#define ELF_PAGEOFFSET(_v) ((_v) & (ELF_MIN_ALIGN-1))
#define ELF_PAGEALIGN(_v) (((_v) + ELF_MIN_ALIGN - 1) & ~(ELF_MIN_ALIGN - 1))

#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define	ET_NUM		5		/* Number of defined types */

#define PROT_READ	0x1		/* page can be read */
#define PROT_WRITE	0x2		/* page can be written */
#define PROT_EXEC	0x4		/* page can be executed */
#define PROT_SEM	0x8		/* page may be used for atomic ops */
#define PROT_NONE	0x0		/* page can not be accessed */
#define PROT_GROWSDOWN	0x01000000	/* mprotect flag: extend change to start of growsdown vma */
#define PROT_GROWSUP	0x02000000	/* mprotect flag: extend change to end of growsup vma */

#define MAP_EXECUTABLE	0x1000		/* mark it as an executable */
#define MAP_SHARED	0x01		/* Share changes */
#define MAP_PRIVATE	0x02		/* Changes are private */
#define MAP_TYPE	0x0f		/* Mask for type of mapping */
#define MAP_FIXED	0x10		/* Interpret addr exactly */
#define MAP_ANONYMOUS	0x20		/* don't use a file */
void xfree(void * ptr, size_t size);
void * xmalloc(size_t size);
void * xrealloc(void * old_ptr, size_t old_size, size_t new_size);
void * xcalloc(size_t nmenb, size_t size);

void *
xcalloc(size_t nmenb, size_t size)
{
	size_t sz;
	assert(nmenb > 0);
	assert(size > 0);
	TRACE(MEM, "allocating 0x%lx bytes of memory\n", (long int)nmenb * size);
	sz = ALIGN_UP(nmenb * size, 4);
	int prot = PROT_READ | PROT_WRITE;
	void * ptr = sys_uniform_mmap(NULL, sz, prot,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(is_valid_ptr(ptr));
	TRACE(MEM, "allocated 0x%lx bytes of memory %p\n", (long int)nmenb * size, ptr);
	return ptr;
}
void *
xmalloc(size_t size)
{
	size_t sz;
	assert(size > 0);
	TRACE(MEM, "allocating 0x%lx bytes of memory\n", (long int)size);
	sz = ALIGN_UP(size , 4);
	int prot = PROT_READ | PROT_WRITE;
	void *ptr = sys_uniform_mmap(NULL, sz, prot,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(is_valid_ptr(ptr));
	TRACE(MEM, "allocated 0x%lx bytes of memory %p\n", (long int)size, ptr);
	return ptr;
}

/*
The  realloc() function changes the size of the memory block pointed to by ptr
to size bytes.  The contents will be unchanged in the range from the start of
the region up to the minimum of the old and new sizes.  If the new size is
larger than the old size, the added memory will not be initial‐ized.  If ptr is
NULL, then the call is equivalent to malloc(size), for all values of size; if
size is equal to zero, and ptr is not NULL, then the call is equivalent to
free(ptr).  Unless ptr is NULL, it must have been returned by an earlier call to
malloc(), calloc()  or  realloc().   If  the area pointed to was moved, a
free(ptr) is done.
*/
void *
xrealloc(void *old_ptr, size_t old_size, size_t size)
{
	void *ptr = NULL;

	if (ptr == NULL) {
		return xmalloc(size);
	}
	if (size == 0 && ptr != NULL) {
		xfree(old_ptr, old_size);
	}
	if (size > old_size) {
		TRACE(MEM, "reallocating 0x%lx bytes of memory at %p\n", (long int)size, ptr);
		size = ALIGN_UP(size , 4);
		void *ptr = sys_mremap(old_ptr, old_size, size,
				MAP_PRIVATE | MAP_ANONYMOUS, old_ptr);
		assert(is_valid_ptr(ptr));
		TRACE(MEM, "allocated 0x%lx bytes of memory %p\n", (long int)size, ptr);
		return ptr;
	}
	// FIXME size < old_size not supported 
	return old_ptr;
}

void
xfree(void * ptr, size_t sz)
{
	assert(is_valid_ptr(ptr));
	TRACE(MEM, "deallocating 0x%lx bytes of pages: %p\n", (long int)sz, ptr);
	sz = ALIGN_UP(sz,4);
	int err = sys_munmap(ptr, sz);
	assert(err == 0);
}

void *xmemset(void *s, int c, size_t count)
{
	char *xs = s;

	while (count--)
		*xs++ = c;
	return s;
}


#define LOG_DIR "/tmp/snitchaser/"
int
xlogger_init(pid_t pid)
{

	int fd;
	char filename[128];
	struct timeval tv;
	int err = sys_gettimeofday(&tv, NULL);
	CASSERT(err == 0, LOGGER, "sys_gettimeofday failed\n");
	size_t fn_sz = snprintf(filename, 128 - 1, \
			LOG_DIR"%d-%d-%010u-%010u.log", sys_getpid(), sys_gettid(), \
			(uint32_t)(tv.tv_sec), (uint32_t)(tv.tv_usec));
	CASSERT(fn_sz < 128 - 1, LOGGER, "fn_sz <> 128\n");
	fd = sys_open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0){
		WARNING(LOADER, "open ckpt file %s failed\n", filename);
		return 1;
	}
	return fd;
}

int
xmemckpt_init(pid_t pid)
{

	int fd;
	char filename[128];
	struct timeval tv;
	int err = sys_gettimeofday(&tv, NULL);
	CASSERT(err == 0, LOGGER, "sys_gettimeofday failed\n");
	size_t fn_sz = snprintf(filename, 128 - 1, \
			LOG_DIR"%d-%d-%010u-%010u.ckpt", sys_getpid(), sys_gettid(), \
			(uint32_t)(tv.tv_sec), (uint32_t)(tv.tv_usec));
	CASSERT(fn_sz < 128 - 1, LOGGER, "fn_sz > 128 bytes\n");
	fd = sys_open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0){
		WARNING(LOADER, "open ckpt file %s failed\n", filename);
		return 1;
	}
	return fd;
}

int SCOPE ckpt_fd = 0;
int SCOPE logger_fd = 0;
uint32_t SCOPE logger_sz = 0;

__attribute__((used, unused, visibility("hidden"))) void
snitchaser_main(struct snitchaser_startup_stack * stack)
{
	DEBUG(SYSTEM, "rebranch start\n");
	struct tls_desc * td = alloc_tls_area();
	active_tls(td);

	struct thread_private_data * tpd = setup_self_tpd();	
	pid_t pid = sys_getpid();
	pid_t tid = sys_gettid();
	tpd->pid = pid;
	tpd->tid = tid;

	init_self_mm_obj(TRUE);

	init_self_bigbuffer();	

	logger_fd = xlogger_init(sys_getpid());
//	ckpt_fd = xmemckpt_init(sys_getpid());
//	flush_mem_to_ckpt(ckpt_fd);

#if 1 
	patch_user_entry();
#endif
}
