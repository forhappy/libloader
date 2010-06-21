/* 
 * snitchaser_args.h
 * by WN @ Apr. 16, 2010
 */

#ifndef __SNITCHASER_OPTS_H
#define __SNITCHASER_OPTS_H

struct opts {
	char * interp_so_fn;
	char * ckpt_fn;
	char * log_fn;
	char * out_log_fn;
	bool_t uncompress_log;
	const char * interp_so_full_name;		/* set according to interp_so_fn */
	int cmd_idx;
	int read_ckpt;
	int gdbserver_debug;
	int gdbserver_remote_debug;
	const char * gdbserver_comm;
};

struct opts *
parse_args(int argc, char * argv[]);

#endif

// vim:ts=4:sw=4

