/* 
 * snitchaser_args.h
 */

struct opts {
	char * inj_so;
	char * wrap_sym;
	char * img_fn;
	char * log_fn;
	char * entry;
	char * old_vsyscall;
	char * state_vect;
	char * injector_opts;
	char * sigreturn;
	char * rt_sigreturn;
	char * sighandler;
	char * ckpt_fn;	/* for gdbloader only */
	char * pthread_so_fn;	/* libpthread.so, for gdbloader only */
	int fix_pthread_tid;	/* whether to fix libpthread's tid, for gdbloader only */
	uint32_t inj_bias;
	int logger_threshold;
	int cmd_idx;
	int trace_fork;
	int untraced;
	int nowait;
};

extern struct opts *
parse_args(int argc, char * argv[]);

// vim:ts=4:sw=4

