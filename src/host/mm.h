/* 
 * mm.h
 * by WN @ Nov. 19, 2009
 */

#ifndef SNITCHASER_MM_H
#define SNITCHASER_MM_H

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <host/exception.h>
#include <stdlib.h>


#define xmalloc(___sz)	({	\
		void * ___ptr;		\
		___ptr = malloc((___sz));\
		if (___ptr == NULL)	\
			THROW_FATAL(EXP_OUT_OF_MEMORY, "out of memory");\
		___ptr; \
		})

#define xmemalign(___boundary, ___sz)	({	\
		void * ___ptr;		\
		___ptr = memalign((___boundary), (___sz));\
		if (___ptr == NULL)	\
			THROW_FATAL(EXP_OUT_OF_MEMORY, "out of memory");\
			___ptr; \
		})


#define xrealloc(___old_ptr, ___new_sz)	({	\
		void * ___ptr;		\
		___ptr = realloc(___old_ptr, ___new_sz);\
		if (___ptr == NULL)	\
			THROW_FATAL(EXP_OUT_OF_MEMORY, "out of memory");\
			___ptr; \
		})

#define xcalloc(___count, ___eltsize)	({	\
		void * ___ptr;		\
		___ptr = calloc((___count), (___eltsize));\
		if (___ptr == NULL)	\
			THROW_FATAL(EXP_OUT_OF_MEMORY, "out of memory");\
			___ptr; \
		})

#define xfree(___ptr) do {	\
	void * ___X_ptr = (void*)(___ptr);	\
	if ((___X_ptr) != NULL)			\
		free((___X_ptr));			\
} while(0)

#define xfree_null(___ptr) do {	\
	xfree(___ptr);					\
	(___ptr) = NULL;					\
} while(0)

#define xfree_null_catched(___ptr) do {	\
	xfree(___ptr);					\
	set_catched_var(___ptr, NULL);	\
} while(0)


#endif
// vim:ts=4:sw=4:cino=l1,\:0

