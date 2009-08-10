#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>
#include <stdint.h>
#include "currf2_args.h"


const char *argp_program_version = "currf2-0.0";
const char *argp_program_bug_address = "<wangnan06@ict.ac.cn>";
static char doc[] =
	"currf2: continuous log/replay";
static char args_doc[] =
	"TARGET-EXECUTABLE [TARGET OPTIONS]";

static struct argp_option options[] = {
	{"injectso",	'j', "filename",	0, "Set the system wrapper so file name, "
											"'./libwrapper.so' by default"},
	{"bias",		'b', "bias",		0, "Set the inject so load bias, 0x3000 by default"},
	{"syswrapper",	'w', "wrapsym",		0, "System call wrapper symbol, syscall_wrapper_entry by default"},
	{"entry",		'E', "entry",		0, "entry, \'__entry\' by default"},
	{"vsyscall",	'e', "vsyscall",	0, "symbol which hold syscall, \'__vsyscall\' by default"},
	{"statevect",	'v', "statevect",	0, "state_vect's symbol, \'state_vector\' by default"},
	{"loggersize",	's', "size",		0, "logger size threshold, 10MB by default, at least 4kB"},
	{"sigreturn",	-1, "sigreturn sym",
		0,  "wrapped_sigreturn symbol, \"wrapped_sigreturn\" by default"},
	{"rt_sigreturn", -2, "rt_sigaction sym", 
		0, "wrapped_rt_sigreturn symbol, \"wrapped_rt_sigreturn\" by default"},
	{"tracefork",	'f', NULL,			0, "trace fork, default: off"},
	{"injopts",		-3, "opts sym",	0, "injector opts symbol, \"injector_opts\" by default"},
	{"untraced",	-4, NULL,			0, "start untraced running. traced by default. "
		"This option is designed for debug use only."},
	{NULL},
};

static struct opts opts = {
	.inj_so		= "./libwrapper.so",
	.wrap_sym 	= "syscall_wrapper_entry",
	.inj_bias	= 0x3000,
	.entry		= "__entry",
	.old_vsyscall	= "__vsyscall",
	.state_vect		= "state_vector",
	.injector_opts	= "injector_opts",
	.sigreturn		= "wrapped_sigreturn",
	.rt_sigreturn	= "wrapped_rt_sigreturn",
	.logger_threshold	= 10 << 20,
	.trace_fork		= 0,
	.untraced		= 0,
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
	static int found_target = 0;
	switch (key) {
		case -4:
			opts.untraced = 1;
			return 0;
		case -3:
			opts.injector_opts = arg;
			return 0;
		case 'f':
			opts.trace_fork = 1;
			return 0;
		case -1:
			opts.sigreturn= arg;
			return 0;
		case -2:
			opts.rt_sigreturn = arg;
			return 0;
		case 's':
			{
				char * t, s;
				errno = 0;
				int sz = strtoul(arg, &t, 10);

				s = toupper(*t);
				if (s == 'K')
					sz *= 1024;
				if (s == 'M')
					sz *= (1 << 20);

				if ((sz < 4096) || (errno != 0)) {
					printf("logger size parse error: at least 4kB\n");
					argp_usage(state);
					return EINVAL;
				}

				opts.logger_threshold = sz;
			}
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
		case 'e':
			opts.old_vsyscall = arg;
			return 0;
		case 'v':
			opts.state_vect = arg;
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

