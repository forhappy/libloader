/* 
 * currf2_args.h
 */

struct opts {
	char * inj_so;
	char * wrap_sym;
	char * img_fn;
	char * log_fn;
	char * entry;
	char * old_vsyscall;
	uint32_t inj_bias;
	int cmd_idx;
};

extern struct opts *
parse_args(int argc, char * argv[]);

// vim:ts=4:sw=4

