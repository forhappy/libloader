/* 
 * snitchaser_args.h
 * by WN @ Apr. 16, 2010
 */

#ifndef __SNITCHASER_OPTS_H
#define __SNITCHASER_OPTS_H

struct opts {
	char * interp_so_fn;
	char * pthread_so_fn;	/* libpthread.so, for gdbloader only */
	char * ckpt_fn;
	const char * interp_so_full_name;		/* set according to interp_so_fn */
	const char * pthread_so_full_name;	/* set according to pthread_so_fn */
	int fix_pthread_tid;	/* whether to fix libpthread's tid, for gdbloader only */
	int cmd_idx;
};

struct opts *
parse_args(int argc, char * argv[]);

#endif

// vim:ts=4:sw=4

