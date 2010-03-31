/* 
 * x86/xasm/types.h
 * by WN @ Mar. 31, 2010
 */

#ifndef __ASM_TYPES_H
#define __ASM_TYPES_H

#include <common/defs.h>

typedef uint8_t 	__u8;
typedef int8_t 		__s8;
typedef uint16_t 	__u16;
typedef int16_t 	__s16;
typedef uint32_t 	__u32;
typedef int32_t 	__s32;
typedef uint64_t 	__u64;
typedef int64_t 	__s64;

typedef	__u8 u8;
typedef	__s8 s8;
typedef	__u16 u16;
typedef	__s16 s16;
typedef	__u32 u32;
typedef	__s32 s32;
typedef	__u64 u64;
typedef	__s64 s64;


typedef uint16_t __le16;
typedef uint16_t __be16;
typedef uint32_t __le32;
typedef uint32_t __be32;
typedef uint64_t __le64;
typedef uint64_t __be64;


#define __force

#endif

// vim:ts=4:sw=4

