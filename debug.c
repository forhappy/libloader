/* 
 * WN @ Jan 24, 2009
 */

/* Debug facility */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <memory.h>

#include <unistd.h>

#ifdef HAVE_EXECINFO_H
/* backtrace */
# include <execinfo.h>
#endif

#include <stdint.h>
#include <malloc.h>
#include <signal.h>

#define CURRF2_DEBUG_C /* Debug masks */
#include "debug.h"


static FILE * fdebug_out = NULL;
static int colorful_terminal = 0;
static inline void print_backtrace(FILE * fp);

static void
debug_malloc_stats(int signum)
{
#ifndef CURRF2_DEBUG_OFF
	enum debug_level level_save = get_comp_level(MEMORY);
	set_comp_level(MEMORY, VERBOSE);
	show_mem_info();
	set_comp_level(MEMORY, level_save);
#endif
}

static void
debug_close()
{
	if (fdebug_out != NULL)
		fclose(fdebug_out);
	fdebug_out = NULL;
}

#define SYS_FATAL(str...) DEBUG_MSG(FATAL, SYSTEM, str)
static void
debug_backtrace(int signum)
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
	print_backtrace(fdebug_out);

	debug_close();

	signal(signum, SIG_DFL);
	raise(signum);
}

#ifdef debug_init
#undef debug_init
#endif
void
debug_init(const char * filename)
{
	int fd;
	if (filename == NULL)
		fdebug_out = stderr;
	else {
		fdebug_out = fopen(filename, "a");
		/* We have not install SIGABRT yet */
		assert(fdebug_out != NULL);
		atexit(debug_close);
	}

	/* get the fileno of fdebug_out, check whether it is a
	 * terminal */
	fd = fileno(fdebug_out);
	assert(fd >= 0);
	if (isatty(fd)) {
		colorful_terminal = 1;
	} else {
		colorful_terminal = 0;
	}

	/* install signal handlers */
	struct signals_handler {
		int signum;
		void (*handler)(int);
	};
	struct signals_handler handler[4] = {
		{SIGUSR1, debug_malloc_stats},
		{SIGSEGV, debug_backtrace},
		{SIGABRT, debug_backtrace},
		{0, NULL},
	};

	int i;
	for (i = 0; handler[i].signum; i++) {
#ifdef HAVE_SIGACTION
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


#ifdef set_comp_level
#undef set_comp_level
#endif
void
set_comp_level(enum debug_component comp, enum debug_level level)
{
	debug_levels[comp] = level;
}

#ifdef get_comp_level
#undef get_comp_level
#endif
enum debug_level
get_comp_level(enum debug_component comp)
{
	return debug_levels[comp];
}


static const char *
get_comp_name(enum debug_component comp)
{
	return debug_comp_name[comp];
}

static const char *
get_level_name(enum debug_level level)
{
	static char * debug_level_names[NR_DEBUG_LEVELS] = {
#ifdef CURRF2_DEBUG_OFF
		[SILENT] 	= "SILENT",
		[TRACE]		= "TRACE",
		[VERBOSE]	= "VERBOSE",
		[WARNING]	= "WARNING",
		[ERROR]		= "ERROR",
		[FATAL]		= "FATAL",
		[FORCE]		= "FORCE",
#else
		[SILENT] 	= "S",
		[TRACE]		= "T",
		[VERBOSE]	= "V",
		[WARNING]	= "WW",
		[ERROR]		= "EW",
		[FATAL]		= "FW",
		[FORCE]		= "FORCE",
#endif
	};
	if ((level >= 0) && (level < NR_DEBUG_LEVELS))
		return debug_level_names[level];
	else
		return "Unknown";
}

static void
turn_red(void)
{
	if (!colorful_terminal)
		return;
	fprintf(fdebug_out, "%c[1;31m", 0x1b);
}

static void
turn_blue(void)
{
	if (!colorful_terminal)
		return;
	fprintf(fdebug_out, "%c[1;34m", 0x1b);
}


static void
turn_normal(void)
{
	if (!colorful_terminal)
		return;
	fprintf(fdebug_out, "%c[m", 0x1b);
}

#ifdef debug_out
#undef debug_out
#endif

static void
vdebug_out(int prefix, enum debug_level level, enum debug_component comp,
	       const char * func_name, int line_no,
       	       const char * fmt, va_list ap)
{
	/* output to stderr even if haven't init */
	if (fdebug_out == NULL)
		fdebug_out = stderr;

	if (debug_levels[comp] <= level) {
		if (level >= WARNING) {
			if (level == FORCE)
				turn_blue();
			else
				turn_red();
		}
		if (prefix) {
			fprintf (fdebug_out, "[%s %s@%s:%d]\t", get_comp_name(comp),
					get_level_name(level), func_name, line_no);
		}

		vfprintf (fdebug_out, fmt, ap);

		if (level >= WARNING)
			turn_normal();
		fflush(fdebug_out);
	}
}

void
debug_out(int prefix, enum debug_level level, enum debug_component comp,
	       const char * func_name, int line_no,
       	       const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vdebug_out(prefix, level, comp, func_name, line_no,
			fmt, ap);
	va_end(ap);
}

/* Define backtrace facilities */
static void * buffer[256];
static inline void print_backtrace(FILE * fp)
{
	if (fp == NULL)
		fp = stderr;
#ifdef HAVE_BACKTRACE
	size_t count;

	count = backtrace(buffer, 256);
	backtrace_symbols_fd(&buffer[1], count-1, fileno(fp));
#endif
	return;
}

void
internal_error(enum debug_component comp,
		const char * func_name,
		int line_no, const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vdebug_out(1, FATAL, comp, func_name, line_no, "INTERNAL ERROR ", ap);
	vdebug_out(0, FATAL, comp, func_name, line_no, fmt, ap);
	va_end(ap);
	raise(SIGABRT);
	/* We shouldn't be here! */
	exit(-1);
}



/* Memory leak detection */
static int malloc_times = 0;
static int calloc_times = 0;
static int free_times = 0;
static int strdup_times = 0;

#define MEM_MSG(str...) DEBUG_MSG(VERBOSE, MEMORY, str)
#define MEM_TRACE(str...) DEBUG_MSG(TRACE, MEMORY, str)

#ifndef show_mem_info
void show_mem_info()
{
	MEM_MSG("malloc times:\t %d\n", malloc_times);
	MEM_MSG("calloc times:\t %d\n", calloc_times);
	MEM_MSG("free times:\t %d\n", free_times);
	MEM_MSG("strdup times:\t %d\n", strdup_times);

#ifdef HAVE_MALLINFO
	MEM_MSG("------ mallinfo ------\n");
	struct mallinfo mi = mallinfo();
	MEM_MSG("System bytes\t=\t\t%d\n", mi.arena+mi.hblkhd);
	MEM_MSG("In use bytes\t=\t\t%d\n", mi.uordblks);
	MEM_MSG("Freed bytes\t=\t\t%d\n", mi.fordblks);
	MEM_MSG("----------------------\n");
#endif

#ifdef HAVE_MALLOC_STATS
	/* FIXME */
	/* malloc_stats output string to stderr. We can
	 * try to temporarily reset stderr to our debug output. */
	malloc_stats();
#endif
#if 0
	if (free_times != malloc_times +
			calloc_times +
			strdup_times)
		MEM_MSG("Memery leak found!!!!\n");
	else
		MEM_MSG("No memory leak found.\n");
#endif
	return;
}
#endif /* show_mem_info */

void * __wrap_malloc(size_t size)
{
	void * res;
	res = malloc(size);
	assert(res != NULL);
	MEM_TRACE("malloc %d, res=%p\n", size, res);
	malloc_times ++;
	return res;
}


void __wrap_free(void * ptr)
{
	MEM_TRACE("free %p\n", ptr);
	free(ptr);
	free_times ++;
	return;
}

void * __wrap_calloc(size_t count, size_t eltsize)
{
	void * res = NULL;
	res = calloc (count, eltsize);
	assert(res != NULL);
	MEM_TRACE("calloc %d x %d\tres=%p\n", count,
			eltsize, res);
	calloc_times ++;
	return res;
}

char * __wrap_strdup(const char * S)
{
	char * res = NULL;
	res = strdup (S);
	assert(res != NULL);
	MEM_TRACE("strdup %s\tres=%p\n", S, res);
	strdup_times ++;
	return res;
}

void * __wrap_realloc(void * ptr, size_t newsize)
{
	void * res;
	res = realloc(ptr, newsize);
	assert(res != NULL);
	MEM_TRACE("realloc %p with new size %d, res=%p\n",
			ptr, newsize, res);
	if (ptr == NULL) {
		malloc_times ++;
	}
	return res;
}

#ifdef CURRF2_DEBUG_OFF
static void
vmessage_out(int prefix, enum debug_level l, enum debug_component c, char * fmt, va_list ap)
{
	/* output to stdout if haven't init */
	if (fdebug_out == NULL)
		fdebug_out = stdout;
	if (l >= WARNING)
		turn_red();

	if (prefix)
		fprintf(fdebug_out, "%s: ", get_level_name(l));
	vfprintf(fdebug_out, fmt, ap);

	if (l >= WARNING)
		turn_normal();
	fflush(fdebug_out);
}

void
message_out(int prefix, enum debug_level l, enum debug_component c, char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vmessage_out(prefix, l, c, fmt, ap);
	va_end(ap);
}
#endif

// vim:tabstop=4:shiftwidth=4

