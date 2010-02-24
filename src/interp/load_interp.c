/* 
 * load_interp.c
 * by WN @ Feb. 24, 2010
 *
 * load the real ld-linux.so
 */

#include <common/debug.h>
#include <common/assert.h>
#include <asm/string.h>
#include <asm/utils.h>
#include <asm/syscall.h>
#include <asm/elf.h>

#define INTERP_FILE		"/lib/ld-linux.so.2"

uintptr_t
load_interp(void * oldesp)
{
	TRACE(LOADER, "loading " INTERP_FILE "\n");
	int fd = INTERNAL_SYSCALL_int80(open, 2,
			INTERP_FILE, O_RDONLY);
	assert(fd >= 0);

	struct elf32_hdr hdr;
	int err = INTERNAL_SYSCALL_int80(read, 3,
			fd, &hdr, sizeof(hdr));

	assert(err == sizeof(hdr));
	assert(memcmp(hdr.e_ident, ELFMAG, SELFMAG) == 0);
	TRACE(LOADER, "magic check ok\n");


	INTERNAL_SYSCALL_int80(close, 1,
			fd);
}

// vim:ts=4:sw=4

