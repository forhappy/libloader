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
	char * state_vect;
	char * ckpt_fn;	/* for gdbloader only */
	uint32_t inj_bias;
	int logger_threshold;
	int cmd_idx;
};

extern struct opts *
parse_args(int argc, char * argv[]);

// vim:ts=4:sw=4

