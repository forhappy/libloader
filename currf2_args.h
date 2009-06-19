/* 
 * currf2_args.h
 */

struct opts {
	char * inj_so;
	char * init_sym;
	char * wrap_sym;
	char * trig_sym;
	char * old_ventry_sym;
	char * old_vhdr_sym;
	uint32_t inj_bias;
	int cmd_idx;
};

extern struct opts *
parse_args(int argc, char * argv[]);

// vim:ts=4:sw=4

