/* 
 * load_interp.c
 * by WN @ Feb. 24, 2010
 *
 * load the real ld-linux.so
 */

#include <common/debug.h>
#include <common/assert.h>
#include <alloca.h>
#include <asm/string.h>
#include <asm/utils.h>
#include <asm/syscall.h>
#include <asm/elf.h>

#define INTERP_FILE		"/lib/ld-linux.so.2"

static int
open_real_interp(struct elf32_hdr * hdr)
{
	int fd = INTERNAL_SYSCALL_int80(open, 2,
			INTERP_FILE, O_RDONLY);
	assert(fd >= 0);

	int err = INTERNAL_SYSCALL_int80(read, 3,
			fd, hdr, sizeof(*hdr));

	assert(err == sizeof(*hdr));
	assert(memcmp(hdr->e_ident, ELFMAG, SELFMAG) == 0);
	TRACE(LOADER, "magic check ok\n");

	/* parse header */
	assert(hdr->e_type == ET_DYN);
	assert(hdr->e_machine == EM_386);
	return fd;
}

static void
load_phdrs(int fd, struct elf32_phdr * dest, struct elf32_hdr * h)
{
	int err;
	err = INTERNAL_SYSCALL_int80(lseek, 3,
			fd, h->e_phoff, SEEK_SET);
	assert(err >= 0);
	err = INTERNAL_SYSCALL_int80(read, 3,
			fd, dest, sizeof(*dest) * h->e_phnum);
	assert(err == sizeof(*dest) * h->e_phnum);
}

static int
compute_total_map_sz(struct elf32_phdr * phdrs, int nr_phdrs)
{
	uint32_t start = 0xffffffff;
	uint32_t end = 0;
	for (int i = 0; i < nr_phdrs; i++) {
		struct elf32_phdr * p = &phdrs[i];
		if (p->p_type != PT_LOAD)
			continue;
		if (start > p->p_vaddr)
			start = p->p_vaddr;
		if (end < p->p_vaddr + p->p_memsz)
			end = p->p_vaddr + p->p_memsz;
	}
	assert(end - start > 0);
	assert(end - start < 0xc0000000);
	return end - start;
}

void *
load_interp(void * oldesp)
{
	TRACE(LOADER, "loading " INTERP_FILE "\n");

	struct elf32_hdr hdr;
	int fd = open_real_interp(&hdr);

	int nr_phdrs = hdr.e_phnum;
	struct elf32_phdr * phdrs = alloca(sizeof(struct elf32_phdr) *
			nr_phdrs);
	assert(phdrs != NULL);
	load_phdrs(fd, phdrs, &hdr);

	int total_map_sz = compute_total_map_sz(phdrs, nr_phdrs);
	TRACE(LOADER, "total mem sz of " INTERP_FILE " = 0x%x\n",
			total_map_sz);

	/* map the file */
	void * map_addr = NULL;

	INTERNAL_SYSCALL_int80(close, 1,
			fd);
	return map_addr + hdr.e_entry;
}

// vim:ts=4:sw=4

