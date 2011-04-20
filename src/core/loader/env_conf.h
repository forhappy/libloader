/* 
 * env_conf.h
 * by WN @ Oct. 18, 2010
 * modified by HP.Fu @ Apr. 5, 2011
 */

#ifndef LOADER_ENV_CONF_H
#define LOADER_ENV_CONF_H

#include <config.h>
#include <defs.h>

struct env_conf {
#define bool_conf(envvar, name, def)	bool_t name;
#define int_conf(envvar, name, def)	int name;
#define __SNITCHASER_ENV_CONF_ENTRIES_H
#include "env_conf_entries.h"
#undef __SNITCHASER_ENV_CONF_ENTRIES_H
#undef int_conf
#undef bool_conf
};

extern struct env_conf env_conf;

#define CONF_VAL(name)	(env_conf.name)

extern void
read_env_conf(void);

#endif

// vim:ts=4:sw=4

