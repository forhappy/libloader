/* 
 * snitchaser_opts.c
 * by WN @ Apr. 16, 2010
 */

#include <common/defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>
#include <stdint.h>
#include "snitchaser_opts.h"

const char *argp_program_version = "snitchaser-2.0";
const char *argp_program_bug_address = "<wangnan06@ict.ac.cn>";
static char doc[] =
	"snitchaser: replayer, load checkpoint and begin replay";
static char args_doc[] =
	"[OPTIONS]";


static struct argp_option options[] = {
	{"libinterpso",	'i', "libinterpso", 0,
		"the file of desired libinterp.so", 0},
	{"ckptfn",		'c', "checkpoint filename", 0,
		"set name of checkpoint file", 0},
	{"readckpt",	'r', NULL, 0,
		"don't run target exec, only read and check ckpt", 0},
	{"logfn",		'l', "log filename", 0,
		"set name of log file", 0},
	{"outlogfn",	'o', "output log filename", 0,
		"set name of output log file", 0},
	/* gdbserver options */

	{"gdbserver-debug", -1, NULL, 0,
		"gdbserver --debug", 0},
	{"gdbserver-remote-debug", -2, NULL, 0,
		"gdbserver --remote-debug", 0},
	{"comm",	'm', "COMM", 0,
		"gdbserver COMM, default is \":12345\"", 0},

	/* uncompress log */
	{"uncompress-log", 'u', NULL, 0,
		"uncompress log in '-l'", 0},

	{NULL, '\0', NULL, 0, NULL, 0},
};

static struct opts opts = {
	.ckpt_fn	= NULL,
	.read_ckpt	= 0,
	.gdbserver_debug 		= 0,
	.gdbserver_remote_debug = 0,
	.gdbserver_comm			= ":12345",
	.uncompress_log 		= FALSE,
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state ATTR(unused))
{
	switch (key) {
	case 'r':
		opts.read_ckpt = 1;
		return 0;
	case 'i':
		opts.interp_so_fn = arg;
		return 0;
	case 'c':
		opts.ckpt_fn = arg;
		return 0;
	case 'm':
		opts.gdbserver_comm = arg;
		return 0;
	case 'l':
		opts.log_fn = arg;
		return 0;
	case 'o':
		opts.out_log_fn = arg;
		return 0;
	case 'u':
		opts.uncompress_log = TRUE;
		return 0;
	case -1:
		opts.gdbserver_debug = 1;
		return 0;
	case -2:
		opts.gdbserver_remote_debug = 1;
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

