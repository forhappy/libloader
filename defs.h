/* Wang Nan @ Jan 25, 2009 */

#ifndef DEFS_H
#define DEFS_H
#include <config.h>
#include <sys/cdefs.h>
#include <stdint.h>

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

__BEGIN_DECLS
#ifdef HAVE_STDBOOL_H
typedef	bool bool_t;
# define TRUE true
# define FALSE false
#else
typedef int bool_t;
# define TRUE	(1)
# define FALSE	(!TRUE)
#endif

typedef uint32_t tick_t;
/* dtick is difference between ticks */
typedef int32_t dtick_t;

typedef void * icon_t;

/* Copy code from list.h */
#ifndef offsetof

#if (__GNUC__ == 4)
#define offsetof(TYPE,MEMBER) __builtin_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#endif

#ifndef container_of
# define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define ATTR(x) __attribute__((x))


/* The ability to declare that a function never returns is useful, but
   not really required to compile GDB successfully, so the NORETURN and
   ATTR_NORETURN macros normally expand into nothing.  */

/* If compiling with older versions of GCC, a function may be declared
   "volatile" to indicate that it does not return.  */

#ifndef NORETURN
#if defined(__GNUC__) \
     && (__GNUC__ == 1 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7))
#define NORETURN volatile
#else
#define NORETURN		/* nothing */
#endif
#endif

/* GCC 2.5 and later versions define a function attribute "noreturn",
   which is the preferred way to declare that a function never returns.
   However GCC 2.7 appears to be the first version in which this fully
   works everywhere we use it. */

#ifndef ATTR_NORETURN
#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7))
#define ATTR_NORETURN ATTR(noreturn)
#else
#define ATTR_NORETURN		/* nothing */
#endif
#endif

__END_DECLS

#endif

// vim:tabstop=4:shiftwidth=4

