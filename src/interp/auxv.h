/* 
 * auxv.h
 * by WN @ Mar. 08, 2010
 */

#ifndef __AUXV_H
#define __AUXV_H

#include <xasm/elf.h>
#include <common/linux/auxvec.h>

void find_auxv(void * oldesp);

struct auxv_info {
	/* entries in aux vector */
	void ** p_user_entry;;
	struct elf32_phdr ** ppuser_phdrs;
	int * p_nr_user_phdrs;
	void ** p_base;
	const char ** p_execfn;
};

extern struct auxv_info auxv_info;

#endif

// vim:ts=4:sw=4

