/* 
 * This file is used to control configure options in environment
 * variables.
 */
#include "string_32.h"

#ifndef __REBRANCH_ENV_CONF_ENTRIES_H
# error Never include <common/config_entries.h> directly
# error use <common/configure.h> instead
#endif

bool_conf("REBRANCH_TRACE_FORK", trace_fork, FALSE)
bool_conf("REBRANCH_UNTRACE_CLONE", untrace_clone, FALSE)
bool_conf("REBRANCH_USE", use_rebranch, TRUE)
int_conf("REBRANCH_SAMPLING", sampling, -1)

