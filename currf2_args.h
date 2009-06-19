/* 
 * currf2_args.h
 */

struct opts {
	char * inj_so;
	char * init_sym;
	char * wrap_sym;
	char * trig_sym;
	char * vdso_entry_sym;
	char * vdso_hdr_sym;
	uint32_t inj_bias;
	int cmd_idx;
};

extern struct opts *
parse_args(int argc, char * argv[]);

// vim:ts=4:sw=4

