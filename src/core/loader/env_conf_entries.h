/* 
 * This file is used to control configure options in environment
 * variables.
 */
#include <linux/string.h>

#ifndef __SNITCHASER_ENV_CONF_ENTRIES_H
# error Never include <loader/config_entries.h> directly
# error use <common/configure.h> instead
#endif

bool_conf("SNITCHASER_TRACE_FORK", trace_fork, FALSE)
bool_conf("SNITCHASER_UNTRACE_CLONE", untrace_clone, FALSE)
bool_conf("SNITCHASER_USE", use_rebranch, TRUE)
int_conf("SNITCHASER_SAMPLING", sampling, -1)

