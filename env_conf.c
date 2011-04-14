/* 
 * env_conf.c
 * by WN @ Oct. 18, 2010
 */


#include <linux/kernel.h>

#include "xconfig.h"
#include "defs.h"
#include "debug.h"
#include "startup_stack.h"
#include "env_conf.h"

static int
_atoi(const char * x)
{
	int s = 0;
	if (x == NULL)
		return s;

	while ((*x <= '9') && (*x >= '0')) {
		s *= 10;
		s += *x++ - '0';
	}

	return s;
}


struct env_conf env_conf = {
#define bool_conf(envvar, name, def)	.name = def,
#define int_conf(envvar, name, def)	.name = def,
#define __REBRANCH_ENV_CONF_ENTRIES_H
#include "env_conf_entries.h"
#undef __REBRANCH_ENV_CONF_ENTRIES_H
#undef int_conf
#undef bool_conf
};

static void
check_var(const char * entry)
{
	assert(entry != NULL);

#define cmp_get(___key, ___str) ({const char * ___res;	\
		if (strncmp(___key "=", ___str, sizeof(___key "=") - 1) == 0) { \
			___res = ___str + sizeof(___key "=") - 1;		\
		} else {	\
			___res = NULL;	\
		}			\
		___res;		\
})

#define bool_conf(___key, ___item, ___def) do {\
	const char * val;\
	if ((val = cmp_get(___key, entry))) {\
		if (strncmp(val, "0", 2) == 0) {\
			CONF_VAL(___item) = FALSE;\
		} else{\
			CONF_VAL(___item) = TRUE;\
		}\
		DEBUG(LOADER, "conf: %s=%d\n", ___key, CONF_VAL(___item));\
	}\
} while(0);

#define int_conf(___key, ___item, ___def) do {\
	const char * val;\
	if ((val = cmp_get(___key, entry))) {\
		CONF_VAL(___item) = _atoi(val);\
		DEBUG(LOADER, "conf: %s=%d\n", ___key, CONF_VAL(___item));\
	}\
} while(0);

#define __REBRANCH_ENV_CONF_ENTRIES_H
#include "env_conf_entries.h"
#undef __REBRANCH_ENV_CONF_ENTRIES_H

#undef bool_conf
#undef int_conf
}

void
read_env_conf(void)
{
	const char ** ptr = STACK_INFO(p_envs);
	while (*ptr != NULL) {
		check_var(*ptr);
		ptr ++;
	}
}

// vim:ts=4:sw=4

