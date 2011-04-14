#ifndef _I386_STRING_H_
#define _I386_STRING_H_

#include "syscall.h"

#define SCOPE __attribute__((visibility("hidden")))
/* Let gcc decide wether to inline or use the out of line functions */

extern SCOPE char *
strcpy(char *dest, const char *src);

extern SCOPE char *
strncpy(char *dest, const char *src, size_t count);

extern SCOPE char *
strcat(char *dest, const char *src);

extern SCOPE char *
strncat(char *dest, const char *src, size_t count);

extern SCOPE int
strcmp(const char *cs, const char *ct);

extern SCOPE int
strncmp(const char *cs, const char *ct, size_t count);

extern SCOPE char *
strchr(const char *s, int c);

extern SCOPE size_t
strlen(const char *s);

static __always_inline void * __memcpy(void * to, const void * from, size_t n)
{
int d0, d1, d2;
__asm__ __volatile__(
	"rep ; movsl\n\t"
	"movl %4,%%ecx\n\t"
	"andl $3,%%ecx\n\t"
	"jz 1f\n\t"
	"rep ; movsb\n\t"
	"1:"
	: "=&c" (d0), "=&D" (d1), "=&S" (d2)
	: "0" (n/4), "g" (n), "1" ((long) to), "2" ((long) from)
	: "memory");
return (to);
}

/*
 * This looks ugly, but the compiler can optimize it totally,
 * as the count is constant.
 */
static __always_inline void * __constant_memcpy(void * to, const void * from, size_t n)
{
	long esi, edi;
	if (!n) return to;
#if 1	/* want to do small copies with non-string ops? */
	switch (n) {
		case 1: *(char*)to = *(char*)from; return to;
		case 2: *(short*)to = *(short*)from; return to;
		case 4: *(int*)to = *(int*)from; return to;
#if 1	/* including those doable with two moves? */
		case 3: *(short*)to = *(short*)from;
			*((char*)to+2) = *((char*)from+2); return to;
		case 5: *(int*)to = *(int*)from;
			*((char*)to+4) = *((char*)from+4); return to;
		case 6: *(int*)to = *(int*)from;
			*((short*)to+2) = *((short*)from+2); return to;
		case 8: *(int*)to = *(int*)from;
			*((int*)to+1) = *((int*)from+1); return to;
#endif
	}
#endif
	esi = (long) from;
	edi = (long) to;
	if (n >= 5*4) {
		/* large block: use rep prefix */
		int ecx;
		__asm__ __volatile__(
			"rep ; movsl"
			: "=&c" (ecx), "=&D" (edi), "=&S" (esi)
			: "0" (n/4), "1" (edi),"2" (esi)
			: "memory"
		);
	} else {
		/* small block: don't clobber ecx + smaller code */
		if (n >= 4*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
		if (n >= 3*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
		if (n >= 2*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
		if (n >= 1*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
	}
	switch (n % 4) {
		/* tail */
		case 0: return to;
		case 1: __asm__ __volatile__("movsb"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
			return to;
		case 2: __asm__ __volatile__("movsw"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
			return to;
		default: __asm__ __volatile__("movsw\n\tmovsb"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
			return to;
	}
}

 
#define memcpy(t, f, n) \
(__builtin_constant_p(n) ? \
 __constant_memcpy((t),(f),(n)) : \
 __memcpy((t),(f),(n)))

void *memmove(void * dest,const void * src, size_t n);


extern void *memchr(const void * cs,int c,size_t count);

static inline void * __memset_generic(void * s, char c,size_t count)
{
int d0, d1;
__asm__ __volatile__(
	"rep\n\t"
	"stosb"
	: "=&c" (d0), "=&D" (d1)
	:"a" (c),"1" (s),"0" (count)
	:"memory");
return s;
}

/* we might want to write optimized versions of these later */
#define __constant_count_memset(s,c,count) __memset_generic((s),(c),(count))

/*
 * memset(x,0,y) is a reasonably common thing to do, so we want to fill
 * things 32 bits at a time even when we don't know the size of the
 * area at compile-time..
 */
static __always_inline void * __constant_c_memset(void * s, unsigned long c, size_t count)
{
int d0, d1;
__asm__ __volatile__(
	"rep ; stosl\n\t"
	"testb $2,%b3\n\t"
	"je 1f\n\t"
	"stosw\n"
	"1:\ttestb $1,%b3\n\t"
	"je 2f\n\t"
	"stosb\n"
	"2:"
	:"=&c" (d0), "=&D" (d1)
	:"a" (c), "q" (count), "0" (count/4), "1" ((long) s)
	:"memory");
return (s);	
}

/* Added by Gertjan van Wingerde to make minix and sysv module work */
extern SCOPE size_t
strnlen(const char *s, size_t count);

extern SCOPE int
memcmp (const void *A1, const void *A2, size_t SIZE);


/* end of additional stuff */

/* extern char *strstr(const char *cs, const char *ct); */

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as we by now know that both pattern and count is constant..
 */
static __always_inline void * __constant_c_and_count_memset(void * s, unsigned long pattern, size_t count)
{
	switch (count) {
		case 0:
			return s;
		case 1:
			*(unsigned char *)s = pattern & 0xff;
			return s;
		case 2:
			*(unsigned short *)s = pattern & 0xffff;
			return s;
		case 3:
			*(unsigned short *)s = pattern & 0xffff;
			*(2+(unsigned char *)s) = pattern & 0xff;
			return s;
		case 4:
			*(unsigned long *)s = pattern;
			return s;
	}
#define COMMON(x) \
__asm__  __volatile__( \
	"rep ; stosl" \
	x \
	: "=&c" (d0), "=&D" (d1) \
	: "a" (pattern),"0" (count/4),"1" ((long) s) \
	: "memory")
{
	int d0, d1;
	switch (count % 4) {
		case 0: COMMON(""); return s;
		case 1: COMMON("\n\tstosb"); return s;
		case 2: COMMON("\n\tstosw"); return s;
		default: COMMON("\n\tstosw\n\tstosb"); return s;
	}
}
  
#undef COMMON
}

#define __constant_c_x_memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_c_and_count_memset((s),(c),(count)) : \
 __constant_c_memset((s),(c),(count)))

#define __memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_count_memset((s),(c),(count)) : \
 __memset_generic((s),(c),(count)))

#define memset(s, c, count) \
(__builtin_constant_p(c) ? \
 __constant_c_x_memset((s),(0x01010101UL*(unsigned char)(c)),(count)) : \
 __memset((s),(c),(count)))

/*
 * find the first occurrence of byte 'c', or 1 past the area if none
 */
extern SCOPE void *
memscan(void * addr, int c, size_t size);

#endif
