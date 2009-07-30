#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>
#include <stdint.h>
#include "currf2_args.h"


const char *argp_program_version = "gdbloader-0.0";
const char *argp_program_bug_address = "<wangnan06@ict.ac.cn>";
static char doc[] =
	"gdbloader: load checkpoint made by currf2, then spin, make gdb can attach to it";
static char args_doc[] =
	"CKPT-FILE TARGET-EXECUTABLE";

static struct argp_option options[] = {
	{"injectso",	'j', "filename",	0, "Set the system wrapper so file name, "
											"'./libwrapper.so' by default"},
	{"bias",		'b', "bias",		0, "Set the inject so load bias, 0x3000 by default"},
	{"syswrapper",	'w', "wrapsym",		0, "System call wrapper symbol, syscall_wrapper_entry by default"},
	{"debugentry",	'E', "entry",		0, "debug_entry, \'__debug_entry\' by default"},
	{"statevect",	'v', "statevect",	0, "state_vect's symbol, \'state_vector\' by default"},
	{"pthreadso",	't', "pthreadso",	0, "desired libpthread.so file name, \'/lib/libpthread.so.0\' by default"},
	{"fixpthread",	'f', NULL,			0, "fix libpthreaed tid and pid"},
	{NULL},
};

static struct opts opts = {
	.inj_so		= "./libwrapper.so",
	.wrap_sym 	= "syscall_wrapper_entry",
	.inj_bias	= 0x3000,
	.entry		= "__debug_entry",
	.state_vect		= "state_vector",
	.pthread_so_fn	= "/lib/libpthread.so.0",
	.fix_pthread_tid = 0,
	.ckpt_fn	= NULL,
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
	static int found_target = 0;
	switch (key) {
		case 'f':
			opts.fix_pthread_tid = 1;
			return 0;
		case 't':
			opts.pthread_so_fn = arg;
			return 0;
		case 'j':
			opts.inj_so = arg;
			return 0;
		case 'b':
			opts.inj_bias = strtol(arg, NULL, 0);
			return 0;
		case 'w':
			opts.wrap_sym = arg;
			return 0;
		case 'E':
			opts.entry = arg;
			return 0;
		case ARGP_KEY_ARG:
			/* ckpt fn arg */
			if (opts.ckpt_fn == NULL) {
				opts.ckpt_fn = arg;
				return 0;
			}
			return ARGP_ERR_UNKNOWN;
		case ARGP_KEY_ARGS:
			/* consume all args */
			return ARGP_ERR_UNKNOWN;
		case ARGP_KEY_END:
			if (!found_target) {
				argp_usage(state);
				return EINVAL;
			}
			return 0;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

struct opts *
parse_args(int argc, char * argv[])
{
	int idx;
	int err;
	err = argp_parse(&argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	opts.cmd_idx = idx;
	return &opts;
}

// vim:ts=4:sw=4

