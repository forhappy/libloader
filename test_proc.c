/*
 * =====================================================================================
 *
 *       Filename:  test_proc.c
 *
 *    Description:  test proc utils
 *
 *        Version:  1.0
 *        Created:  04/04/2011 11:33:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: HP.Fu 
 *        Company:  
 *
 * =====================================================================================
 */
#include "xconfig.h"
#include "debug.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"
#include <stdio.h>
//#include <stdlib.h>

int main(int argc, char * argv[])
{
	dbg_init();
	DEBUG(SYSTEM, "init\n");
	assert(argc >= 2);

	char fn[256];
	snprintf(fn, 256, "/proc/%s/maps", argv[1]);
	DEBUG(SYSTEM, "map file: %s\n", fn);

	FILE * fp = fopen(fn, "rb");
	assert(fp != NULL);

	char * buffer = calloc(1, 1 << 20);
	assert(buffer != NULL);
	int sz = fread(buffer, 1, 1 << 20, fp);
	assert(sz > 0);
	assert(sz < (1 << 20));

	printf("%s\n", buffer);

	struct proc_mem_handler_t h;
	init_map_handler(&h, buffer, sz);

	struct proc_maps_entry_t r;
	r = read_map_line(&h);
	while (r.valid) {
		printf("%p-%p \t %x, %016llx ",
				(void*)r.start, (void*)r.end, r.prot,
				(unsigned long long)r.offset);
		if (r.fn != NULL) {
			assert(r.fn[r.fn_len] == '\n');
			r.fn[r.fn_len] = '\0';
			printf("|%s|", r.fn);
		}
		printf("\n");
		r = read_map_line(&h);
	}

	if (argv[2] != NULL) {
		r = find_in_map_by_fn(&h, argv[2], TRUE);
		if (r.valid)
			printf("found: 0x%lx-0x%lx\n", (unsigned long)r.start, (unsigned long)r.end);
		else
			printf("not found\n");
	}

	r = read_map_line(&h);
	while (r.valid) {
#ifdef ARCH_X86_32
		printf("%x-%x \t %x, %08llx ",
				r.start, r.end, r.prot, r.offset);
#endif
#ifdef ARCH_X86_64
		printf("%lx-%lx \t %x, %08lx ",
				r.start, r.end, r.prot, r.offset);
#endif

		if (r.fn != NULL) {
			assert(r.fn[r.fn_len] == '\0');
			r.fn[r.fn_len] = '\0';
			printf("|%s|", r.fn);
		}
		printf("\n");
		r = read_map_line(&h);
	}

	return 0;
}
