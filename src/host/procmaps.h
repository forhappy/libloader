/* 
 * procmap.h
 * by WN @ Apr. 19, 2010
 */

#ifndef __SNITCHASER_PROCMAP_H
#define __SNITCHASER_PROCMAP_H

#include <stdint.h>
#include <unistd.h>

#include <common/defs.h>

char *
proc_maps_load(pid_t pid);

void
proc_maps_free(char * proc_data);

struct proc_mem_region {
	uintptr_t start;
	uintptr_t end;
	uint32_t prot;
	uint32_t offset;
};

/* return true: found */
/* return false: not found or error */
bool_t
proc_maps_find(struct proc_mem_region * preg,
		const char * filename, char * proc_data);

#endif

// vim:ts=4:sw=4

