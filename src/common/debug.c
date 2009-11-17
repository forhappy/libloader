/* 
 * debug.c
 * by WN @ Nov. 08, 2009
 */


#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>

#ifdef HAVE_EXECINFO_H
/* backtrace */
# include <execinfo.h>
#endif

#define __DEBUG_C
#include <common/defs.h>
#include <common/debug.h>

#ifdef SNITCHASER_DEBUG
static const char * __debug_level_names[NR_DEBUG_LEVELS] = {
	[DBG_LV_SILENT]		= "SIL",
	[DBG_LV_TRACE]		= "TRC",
	[DBG_LV_DEBUG]		= "DBG",
	[DBG_LV_VERBOSE]	= "VRB",
	[DBG_LV_WARNING]	= "WAR",
	[DBG_LV_ERROR]		= "ERR",
	[DBG_LV_FATAL]		= "FAL",
	[DBG_LV_FORCE]		= "FOC",
};
#endif

#define get_level_name(l)	(__debug_level_names[l])
#define get_comp_name(c)	(__debug_component_names[c])
#define get_comp_level(c)	(__debug_component_levels[c])

static FILE * output_fp = NULL;
static bool_t colorful_terminal = FALSE;


/* colorful console */
enum color_name {
	COLOR_RED = 0,
	COLOR_BOLD_RED,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_NORMAL,
};

/* about the colorful terminal, see:
 * http://www.pixelbeat.org/docs/terminal_colours/
 */
static const char * color_strs[] = {
	[COLOR_RED]		= "\x001b[31m",
	[COLOR_BOLD_RED]	= "\x001b[1;31m",
	[COLOR_YELLOW]		= "\x001b[33m",
	[COLOR_BLUE]		= "\x001b[34m",
	[COLOR_NORMAL]		= "\x001b[0m",
};

static void
set_color(enum color_name c)
{
	if (!colorful_terminal)
		return;
	fprintf(output_fp, "%s", color_strs[c]);
}
/* end colorful console */

/* memory leak detection */

#ifdef SNITCHASER_DEBUG
static int malloc_counter = 0;
static int calloc_counter = 0;
static int free_counter = 0;
static int strdup_counter = 0;

void *
__wrap_malloc(size_t size,
		__dbg_info_param)
{
	void * res;
	res = malloc(size);
	assert(res != NULL);
	TRACE(MEMORY, "@q malloc(%d)@[%s:%d]=%p\n", size,
			func, line,
			res);
	malloc_counter ++;
	return res;
}

void
__wrap_free(void * ptr,
		__dbg_info_param)
{
	TRACE(MEMORY, "@q free(%p)@[%s:%d]\n", ptr,
			func, line);
	free(ptr);
	free_counter ++;
	return;
}

void *
__wrap_calloc(size_t count, size_t eltsize,
		__dbg_info_param)
{
	void * res = NULL;
	res = calloc (count, eltsize);
	assert(res != NULL);
	TRACE(MEMORY, "@q calloc(%d, %d)@[%s:%d]=%p\n", count, eltsize,
			func, line,
			res);
	calloc_counter ++;
	return res;
}

char *
__wrap_strdup(const char * S,
		__dbg_info_param)
{
	char * res = NULL;
	res = strdup (S);
	assert(res != NULL);
	TRACE(MEMORY, "strdup(%s)@[%s:%d]=%p\n", S,
			func, line,
			res);
	strdup_counter ++;
	return res;
}


void *
__wrap_realloc(void * ptr, size_t newsize,
		__dbg_info_param)
{
	void * res;
	res = realloc(ptr, newsize);
	assert(res != NULL);
	TRACE(MEMORY, "realloc(%p, %d)@[%s:%d]=%p\n", ptr, newsize,
			func, line,
			res);
	if (ptr == NULL) {
		malloc_counter ++;
	}
	return res;
}
#endif

/* end memory leak detection */


/* signal handlers */
#define MEM_MSG(str...)	FORCE(MEMORY, "@q" str)
static void
sighandler_mem_stats(int signum)
{
#ifndef SNITCHASER_DEBUG
	VERBOSE(MEMORY, "this compilation doesn't "
			"support memory status report\n");
#else
	MEM_MSG("------ malloc counters ------\n");
	MEM_MSG("malloc counter:\t%d\n", malloc_counter);
	MEM_MSG("calloc counter:\t%d\n", calloc_counter);
	MEM_MSG("strdup counter:\t%d\n", strdup_counter);
	MEM_MSG("free counter:\t%d\n", free_counter);
#ifdef HAVE_MALLINFO
	MEM_MSG("------ mallinfo ------\n");
	struct mallinfo mi = mallinfo();
	MEM_MSG("System bytes\t=\t\t%d\n", mi.arena+mi.hblkhd);
	MEM_MSG("In use bytes\t=\t\t%d\n", mi.uordblks);
	MEM_MSG("Freed bytes\t=\t\t%d\n", mi.fordblks);
#endif
	MEM_MSG("----------------------\n");
#endif
}
#undef MEM_MSG

/* save stack space */
static void * buffer[256];
static void print_backtrace(FILE * fp)
{
#ifdef HAVE_BACKTRACE
	if (fp == NULL)
		fp = stderr;
	size_t count;

	count = backtrace(buffer, 256);
	backtrace_symbols_fd(&buffer[1], count-1, fileno(fp));
#else
	fprintf(fp, "this compilation doesn't "
			"support backtrace reporting\n");
#endif
	return;
}

#define SYS_FATAL(str...) FATAL_MSG(SYSTEM, str)
static void
sighandler_backtrace(int signum)
{
	switch (signum) {
		case SIGSEGV:
			SYS_FATAL("Received SIGSEGV:\n");
			break;
		case SIGABRT:
			SYS_FATAL("Received SIGABRT:\n");
			break;
		default:
			SYS_FATAL("Received signal %d\n", signum);
			break;
	}
	print_backtrace(output_fp);

	dbg_exit();

	signal(signum, SIG_DFL);
	raise(signum);
}

/* end signal handlers */

void
dbg_exit(void)
{
	if ((output_fp != NULL) &&
		(output_fp != stderr) &&
		(output_fp != stdout))
		fclose(output_fp);
	set_color(COLOR_NORMAL);
	output_fp = NULL;
}

void
dbg_init(const char * fn)
{
	int err;

	/* open output_fp for debug output */
	if ((fn == NULL) || fn[0] == '\0') {
		output_fp = stderr;
	} else {
		output_fp = fopen(fn, "a");
		/* We will install a handler for SIGABRT */
		assert(output_fp != NULL);
#ifdef HAVE_ATEXIT
		atexit(dbg_exit);
#endif
	}
	/* test the colorful terminal */
	if (isatty(fileno(output_fp)))
		colorful_terminal = TRUE;
	else
		colorful_terminal = FALSE;

	/* we close buffer of output_fp */
	err = setvbuf(output_fp, NULL, _IONBF, 0);
	if (err != 0)
		fprintf(stderr, "Failed to set _IONBF to debug output file\n");

	set_color(COLOR_NORMAL);

	/* reset errno for isatty and setvbuf */
	errno = 0;

	/* install singal handlers */
	struct signals_handler {
		int signum;
		void (*handler)(int);
	};
	struct signals_handler handler[4] = {
		{SIGUSR1, sighandler_mem_stats},
		{SIGSEGV, sighandler_backtrace},
		{SIGABRT, sighandler_backtrace},
		{0, NULL},
	};

	int i;
	for (i = 0; handler[i].signum; i++) {
#ifdef HAVE_SIGACTION
		/* there a no different between sigaction and
		 * signal, because we don't need signal mask at all. */
		struct sigaction action;
		sigaction(handler[i].signum, NULL, &action);
		action.sa_handler = handler[i].handler;
		sigaction(handler[i].signum, &action, NULL);
#else
		signal(handler[i].signum,
			       handler[i].handler);
#endif
	}
}

struct msg_option {
	bool_t no_prefix;
};

void
dbg_output(enum __debug_level level,
#ifdef SNITCHASER_DEBUG
		enum __debug_component comp,
		const char * file,
		const char * func,
		int line,
#endif
		char * fmt, ...)
{

	va_list ap;
	va_start(ap, fmt);

	if (fmt == NULL)
		return;
	if (level == DBG_LV_SILENT)
		return;
#ifdef SNITCHASER_DEBUG

	/* check component and level */
	if (get_comp_level(comp) > level)
		return;

	/* check fmt */
	struct msg_option option = {
		.no_prefix = FALSE,
	};
	while (*fmt == '@') {
		fmt ++;
		switch (*fmt) {
			case 'q':
				option.no_prefix = TRUE;
				fmt++;
				break;
			case '@':
				break;
			default:
				set_color(COLOR_RED);
				fprintf(output_fp, 
					"!!! debug option char '%c' error\n",
					*fmt);
				set_color(COLOR_NORMAL);
		}
	}

	if (*fmt == ' ')
		fmt++;

	/* now fmt should point to the start of real fmt */
	switch (level) {
		case DBG_LV_FORCE:
			set_color(COLOR_BLUE);
			break;
		case DBG_LV_WARNING:
			set_color(COLOR_YELLOW);
			break;
		case DBG_LV_ERROR:
			set_color(COLOR_RED);
			break;
		case DBG_LV_FATAL:
			set_color(COLOR_BOLD_RED);
			break;
		default:
			set_color(COLOR_NORMAL);
	}

	/* print the prefix */
	if (!option.no_prefix)
		fprintf(output_fp, "[%s %s@%s:%d]\t",
				get_comp_name(comp),
				get_level_name(level),
				func,
				line);

	/* then real print */
	vfprintf (output_fp, fmt, ap);


	if (level >= DBG_LV_WARNING)
		set_color(COLOR_NORMAL);
#else
	if (level < DBG_LV_VERBOSE)
		return;
	/* remove '@' */
	while (*fmt == '@') {
		fmt ++;
		if (*fmt != '@')
			fmt ++;
		else
			break;
	}
	if (*fmt == ' ')
		fmt++;


	switch (level) {
		case DBG_LV_FORCE:
			set_color(COLOR_BLUE);
			break;
		case DBG_LV_WARNING:
			set_color(COLOR_YELLOW);
			break;
		case DBG_LV_ERROR:
			set_color(COLOR_RED);
			break;
		case DBG_LV_FATAL:
			set_color(COLOR_BOLD_RED);
			break;
		default:
			set_color(COLOR_NORMAL);
	}

	vfprintf(output_fp, fmt, ap);

	if (level >= DBG_LV_WARNING)
		set_color(COLOR_NORMAL);
#endif
	va_end(ap);
}

void NORETURN ATTR_NORETURN
dbg_fatal(void)
{
	fprintf(stderr, "fatal error occured, cannot continue.\n");
	raise(SIGABRT);
	exit(-1);
}

// vim:ts=8:sw=8

