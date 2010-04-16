/* 
 * snitchaser_args.c
 * by WN @ Apr. 16, 2010
 */

#include <common/defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>
#include <stdint.h>
#include "snitchaser_args.h"

const char *argp_program_version = "snitchaser-2.0";
const char *argp_program_bug_address = "<wangnan06@ict.ac.cn>";
static char doc[] =
	"snitchaser: replayer, load checkpoint and begin replay";
static char args_doc[] =
	"[TARGET OPTIONS]";


static struct argp_option options[] = {
	{"pthreadso",	't', "pthreadso",	0, "desired libpthread.so file name, \'/lib/libpthread.so.0\' by default", 0},
	{"ckptfn",		'c', "checkpoint filename", 0, "set checkpoint file name", 0},
	{"targetfn",	'x', "executable filename", 0, "set executable file name", 0},
	{"no-fix-pthread",	-1, NULL,			0, "don't fix libpthreaed tid and pid", 0},
	{NULL, '\0', NULL, 0, NULL, 0},
};

static struct opts opts = {
	.pthread_so_fn	= "/lib/libpthread.so.0",
	.ckpt_fn	= NULL,
	.target_fn	= NULL,
	.fix_pthread_tid = 1,
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state ATTR(unused))
{
	switch (key) {
		case 't':
			opts.pthread_so_fn = arg;
			return 0;
		case 'c':
			opts.ckpt_fn = arg;
			return 0;
		case 'x':
			opts.target_fn = arg;
			return 0;
		case -1:
			opts.fix_pthread_tid = 0;
			return 0;
		case ARGP_KEY_END:
			return 0;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc,
	NULL, NULL, NULL};

struct opts *
parse_args(int argc, char * argv[])
{
	int idx;
	int err;
	err = argp_parse(&argp, argc, argv, ARGP_IN_ORDER, &idx, NULL);
	opts.cmd_idx = idx;
	return &opts;
}

// vim:ts=4:sw=4

