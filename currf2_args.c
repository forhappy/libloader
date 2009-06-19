#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>
#include <stdint.h>
#include "currf2_args.h"


const char *argp_program_version = "currf2-0.0";
const char *argp_program_bug_address = "<pi3orama@gmail.com>";
static char doc[] =
	"currf2: continuous log/replay";
static char args_doc[] =
	"TARGET-EXECUTABLE [TARGET OPTIONS]";

static struct argp_option options[] = {
	{"injectso",	'j', "filename",	0, "Set the system wrapper so file name, "
											"'./syscall_wrapper_entrance.so' by default"},
	{"bias",		'b', "bias",		0, "Set the inject so load bias, 0x3000 by default"},
	{"soinit",		'i', "initsym",		0, "The injected so init symbol, NULL by default"},
	{"syswrapper",	'w', "wrapsym",		0, "System call wrapper symbol, syscall_wrapper_entrace by default"},
	{"trigger",		't', "trigsym",		0, "Trigger symbol, intercept_start by default"},
	{"vdsoentry",	'E', "ventrysym",	0, "vdso entry symbol, NULL by default"},
	{"vdsoehdr",	'H', "vehdrsym",	0, "vdso ehdr symbol, NULL by default"},
	{NULL},
};

static struct opts opts = {
	.inj_so		= "./syscall_wrapper_entrance.so",
	.init_sym	= NULL,
	.wrap_sym 	= "syscall_wrapper_entrace",
	.trig_sym	= "intercept_start",
	.vdso_entry_sym	= NULL,
	.vdso_hdr_sym	= NULL,
	.inj_bias	= 0x3000,
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
	static int found_target = 0;
	switch (key) {
		case 'j':
			opts.inj_so = arg;
			return 0;
		case 'b':
			opts.inj_bias = strtol(arg, NULL, 0);
			return 0;
		case 'i':
			opts.init_sym = arg;
			return 0;
		case 'w':
			opts.wrap_sym = arg;
			return 0;
		case 't':
			opts.trig_sym = arg;
			return 0;
		case 'E':
			opts.vdso_entry_sym = arg;
			return 0;
		case 'H':
			opts.vdso_hdr_sym = arg;
			return 0;
		case ARGP_KEY_ARG:
			/* target args */
			found_target = 1;
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
	err = argp_parse(&argp, argc, argv, ARGP_IN_ORDER, &idx, NULL);
	opts.cmd_idx = idx;
	return &opts;
}

// vim:ts=4:sw=4

