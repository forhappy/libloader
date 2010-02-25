/* 
 * load_interp.c
 * by WN @ Feb. 24, 2010
 *
 * load the real ld-linux.so
 */

#include <common/debug.h>
#include <common/assert.h>


extern void *
load_elf(const char * fn, void ** p_load_bias);

void * real_interp_address = NULL;

void *
load_real_interp(void)
{
	void * load_bias;
	void * entry = load_elf(INTERP_FILE,
			&load_bias);
	assert(entry != NULL);
	real_interp_address = load_bias;
	return entry;
}

// vim:ts=4:sw=4

