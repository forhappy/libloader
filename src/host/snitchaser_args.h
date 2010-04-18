/* 
 * snitchaser_args.h
 * by WN @ Apr. 16, 2010
 */

#ifndef __SNITCHASER_ARGS_H
#define __SNITCHASER_ARGS_H

struct opts {
	char * pthread_so_fn;	/* libpthread.so, for gdbloader only */
	char * ckpt_fn;
	int fix_pthread_tid;	/* whether to fix libpthread's tid, for gdbloader only */
	int cmd_idx;
};

struct opts *
parse_args(int argc, char * argv[]);

#endif

// vim:ts=4:sw=4

