/* 
 * debug.c (host)
 * by WN @ Nov. 1, 2010
 */


#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <execinfo.h>

#define __DEBUG_C
#include <common/debug.h>

static void * buffer[256];
static void print_backtrace(FILE * fp)
{
	if (fp == NULL)
		fp = stderr;
	size_t count;

	count = backtrace(buffer, 256);
	backtrace_symbols_fd(&buffer[1], count - 1, fileno(fp));
	return;
}

static void
sighandler_backtrace(struct siginfo siginfo)
{
	int signum = siginfo.si_signo;

	switch (signum) {
		case SIGSEGV:
			ERROR(SYSTEM, "Received SIGSEGV at %p:\n",
					siginfo.si_addr);
			break;
		case SIGABRT:
			ERROR(SYSTEM, "Received SIGABRT:\n");
			break;
		default:
			ERROR(SYSTEM, "Received signal %d\n", signum);
			break;
	}
	print_backtrace(stderr);

	signal(signum, SIG_DFL);
	raise(signum);
}


void
dbg_init(void)
{
	fflush(stderr);
	struct sigaction act;
	memset(&act, '\0', sizeof(act));
	act.sa_handler = (__sighandler_t)sighandler_backtrace;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, NULL);
	sigaction(SIGABRT, &act, NULL);
}

void
dbg_output(enum __debug_level level,
#ifdef REBRANCH_DEBUG
		enum __debug_component comp,
		const char * file ATTR(unused),
		const char * func,
		int line,
#endif
		char * fmt, ...)
{
	if (fmt == NULL)
		return;
	if (level == DBG_LV_SILENT)
		return;

	assert(level < NR_DEBUG_LEVELS);
#ifdef REBRANCH_DEBUG
	assert(comp < NR_DEBUG_COMPONENTS);
	if (level < __debug_component_levels[comp])
		return;
#endif

#ifdef REBRANCH_DEBUG
	fprintf(stderr, "[%s %s@%s:%d]\t",
			(char*)__debug_component_names[comp],
			(char*)__debug_level_names[level],
			func, line);
#else
	fprintf(stderr, "%s:\t", __debug_level_names[level]);
#endif

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

void ATTR(noreturn)
dbg_fatal(void)
{
	fflush(stdout);
	fflush(stderr);
	raise(SIGABRT);
	exit(-1);
}

#undef __DEBUG_C

// vim:ts=4:sw=4

