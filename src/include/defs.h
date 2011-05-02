/* Wang Nan @ Jan 25, 2009 */

#ifndef DEFS_H
#define DEFS_H
#include <config.h>

#ifndef __KERNEL__
# include <sys/cdefs.h>
# include <stdint.h>
#else
# include <linux/types.h>
# include <linux/kernel.h>
#endif

#include <endian.h>
#include <assert.h>
#include <stddef.h>
#include <alloca.h>

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
#define defined_BOOL_T
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
#define ATTR_HIDDEN ATTR(visibility("hidden"))
#define ATTR_EXPORT ATTR(visibility("default"))
#define __AI	inline ATTR(always_inline)
#define __WUR	ATTR(warn_unused_result)


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

#define ALIGN_DOWN(v, a)	(typeof(v))((uint32_t)(v) & (~((a) - 1)))
#define ALIGN_UP(v, a)		(typeof(v))(ALIGN_DOWN((uint32_t)(v) + (uint32_t)(a) - 1, (a)))

#define ALIGN_DOWN_64(v, a)	(typeof(v))((uint64_t)(v) & (~((a) - 1)))
#define ALIGN_UP_64(v, a)		(typeof(v))(ALIGN_DOWN_64((uint64_t)(v) + (uint64_t)(a) - 1, (a)))

#define ALIGN_DOWN_PTR(v, a)	(typeof(v))((uintptr_t)(v) & (~((a) - 1)))
#define ALIGN_UP_PTR(v, a)		(typeof(v))(ALIGN_DOWN_PTR((uintptr_t)(v) + (uintptr_t)(a) - 1, (a)))

#define read_le16(p)	(le16toh((*((uint16_t*)(p)))))
#define read_le32(p)	(le32toh((*((uint32_t*)(p)))))
#define read_le64(p)	(le64toh((*((uint64_t*)(p)))))

#define tole64(p)	((p) = le64toh((p)))
#define tole32(p)	((p) = le32toh((p)))
#define tole16(p)	((p) = le16toh((p)))

#ifndef max
# define max(a, b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef min
# define min(a, b)	(((a) < (b)) ? (a) : (b))
#endif

#define max0(a, b)	max(a, b)
#define min0(a, b)	((((a) < (b)) && ((a) != 0)) ? (a) : (b))

#define ptr_diff(a, b)	((ptrdiff_t)((uintptr_t)(a) - (uintptr_t)(b)))

#define ptr_as_puint8(p)		((uint8_t*)(p))
#define ptr_as_puint16(p)	((uint16_t*)(p))
#define ptr_as_puint32(p)	((uint32_t*)(p))
#define ptr_as_puint64(p)	((uint64_t*)(p))
#define ptr_as_puintptr(p)	((uintptr_t*)(p))
#define ptr_as_pptrdiff(p)	((ptrdiff_t*)(p))
#define ptr_as_pptrptr(p)	((void**)(p))

#define ptr_as_pint8(p)		((int8_t*)(p))
#define ptr_as_pint16(p)	((int16_t*)(p))
#define ptr_as_pint32(p)	((int32_t*)(p))
#define ptr_as_pint64(p)	((int64_t*)(p))
#define ptr_as_pintptr(p)	((intptr_t*)(p))


#define set_uint8(p, v)		((*(ptr_as_puint8(p))) = (v))
#define set_uint16(p, v)	((*(ptr_as_puint16(p))) = (v))
#define set_uint32(p, v)	((*(ptr_as_puint32(p))) = (v))
#define set_uint64(p, v)	((*(ptr_as_puint64(p))) = (v))
#define set_uintptr(p, v)	((*(ptr_as_puintptr(p))) = (v))
#define set_ptrdiff(p, v)	((*(ptr_as_pptrdiff(p))) = (v))

static inline char *
_strtok(const char * str, char tok)
{
	assert(tok != '\0');
	while ((*str != '\0') && (*str != tok))
		str ++;
	return (char*)str;
}

# define likely(x)	__builtin_expect(!!(x), 1)
# define unlikely(x)	__builtin_expect(!!(x), 0)

#ifndef ATTR_UNUSED
# define ATTR_UNUSED	ATTR(unused)
#endif

#ifdef SNITCHASER_DEBUG
# define DEBUG_ARG
# define DEBUG_DEF
#else
# define DEBUG_ARG ATTR_UNUSED
# define DEBUG_DEF ATTR_UNUSED
#endif

#ifdef SCOPE
# undef SCOPE
#endif
#define SCOPE ATTR(visibility ("hidden"))
__END_DECLS

#define MAX_OUTPUT_FN_LEN	(128)
#define BIGBUFFER_SZ	((2048) * (1024))
#define MAX_PROC_MAPS_FILE_SIZE	((1024) * 1024)/* Copy code from list.h */
#define LOG_DIR "/tmp/snitchaser/"
#endif

// vim:tabstop=4:shiftwidth=4

