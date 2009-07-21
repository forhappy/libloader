/* 
 * ckptutils.h
 * by WN @ Jul. 16, 2008
 * 
 */

#ifndef CKPTUTILS_H
#define CKPTUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "checkpoint/checkpoint.h"
#include "exception.h"
#include "debug.h"

struct ckpt_file {
	struct cleanup clup;
	struct state_vector * state;
	struct user_regs_struct * regs;
	int nr_regions;
	struct mem_region ** regions;
	char ** cmdline;
	void * ckpt_img;
};

extern struct ckpt_file *
load_ckpt_file(char * fn);

void
close_ckpt_file(struct ckpt_file * f);

#endif

