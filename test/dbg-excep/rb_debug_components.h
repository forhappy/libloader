/* 
 * This file is used for control the debugger output.
 * It should be included in debug.h, and never be used directly.
 */

#ifndef __DEBUG_INCLUDE_DEBUG_COMPONENTS_H
# error Never include <common/debug_components.h> directly,
# error use <common/debug.h> instead.
#endif

#if 1
def_dbg_component(SYSTEM, 		"SYS ",	VERBOSE)
def_dbg_component(LOADER, 		"LOAD",	VERBOSE)
def_dbg_component(TLS,	 		"TLS ",	VERBOSE)
def_dbg_component(MUTEX, 		"MUTX",	VERBOSE)
def_dbg_component(DECODER, 		"DECO",	VERBOSE)
def_dbg_component(MEM,	 		"MEM ",	VERBOSE)
def_dbg_component(DICT, 		"DICT",	VERBOSE)
def_dbg_component(CCACHE,	 	"CACH",	VERBOSE)
def_dbg_component(COMPILER, 	"CMPL",	VERBOSE)
def_dbg_component(LOGGER, 		"LOG ",	VERBOSE)
def_dbg_component(CKPT, 		"CKPT",	TRACE)
def_dbg_component(SIGNAL, 		"SIG ",	VERBOSE)
def_dbg_component(COMPRESS,		"LZO ",	VERBOSE)
def_dbg_component(REPLAYER_HOST, "REPH",	VERBOSE)
def_dbg_component(REPLAYER_TARGET, "REPT",	VERBOSE)
def_dbg_component(PTRACE, 		"PTRC",	VERBOSE)
def_dbg_component(PROCMAPS, 	"PMAP",	VERBOSE)
def_dbg_component(ELF, 			"ELF ",	VERBOSE)
def_dbg_component(LOG_SYSCALL, 	"LSYS",	VERBOSE)
def_dbg_component(XGDBSERVER, 	"GDBS",	VERBOSE)
def_dbg_component(THREAD, 		"MT  ",	VERBOSE)
#else
def_dbg_component(SYSTEM, 		"SYS ",	TRACE)
def_dbg_component(LOADER, 		"LOAD",	TRACE)
def_dbg_component(TLS,	 		"TLS ",	TRACE)
def_dbg_component(MUTEX, 		"MUTX",	TRACE)
def_dbg_component(DECODER, 		"DECO",	TRACE)
def_dbg_component(MEM,	 		"MEM ",	TRACE)
def_dbg_component(DICT, 		"DICT",	TRACE)
def_dbg_component(CCACHE,	 	"CACH",	TRACE)
def_dbg_component(COMPILER, 	"CMPL",	TRACE)
def_dbg_component(LOGGER, 		"LOG ",	TRACE)
def_dbg_component(CKPT, 		"CKPT",	TRACE)
def_dbg_component(SIGNAL, 		"SIG ",	TRACE)
def_dbg_component(COMPRESS,		"LZO ",	TRACE)
def_dbg_component(REPLAYER_HOST, "REPH",	TRACE)
def_dbg_component(REPLAYER_TARGET, "REPT",	TRACE)
def_dbg_component(PTRACE, 		"PTRC",	TRACE)
def_dbg_component(PROCMAPS, 	"PMAP",	TRACE)
def_dbg_component(ELF, 			"ELF ",	TRACE)
def_dbg_component(LOG_SYSCALL, 	"LSYS",	TRACE)
def_dbg_component(XGDBSERVER, 	"GDBS",	TRACE)
def_dbg_component(THREAD, 		"MT  ",	TRACE)
#endif

/* set to NULL means stderr */
#ifndef DEBUG_OUTPUT_FILE
# define DEBUG_OUTPUT_FILE	(NULL)
#endif

// 
// vim:ts=4:sw=4:cino=l1,\:0

