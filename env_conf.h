/* 
 * env_conf.h
 * by WN @ Oct. 18, 2010
 */

#ifndef INTERP_ENV_CONF_H
#define INTERP_ENV_CONF_H

#include "xconfig.h"
#include "defs.h"

struct env_conf {
#define bool_conf(envvar, name, def)	bool_t name;
#define int_conf(envvar, name, def)	int name;
#define __REBRANCH_ENV_CONF_ENTRIES_H
#include "env_conf_entries.h"
#undef __REBRANCH_ENV_CONF_ENTRIES_H
#undef int_conf
#undef bool_conf
};

extern struct env_conf env_conf;

#define CONF_VAL(name)	(env_conf.name)

extern void
read_env_conf(void);

#endif

// vim:ts=4:sw=4

