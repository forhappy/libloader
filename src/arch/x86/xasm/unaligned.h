/* 
 * x86/xasm/unaligned.h
 * by WN @ Mar. 31, 2010
 */

#ifndef __ASM_UNALIGNED_H
#define __ASM_UNALIGNED_H

#include <xasm/byteorder.h>

static inline u16 get_unaligned_le16(const void *p)
{
	return le16_to_cpup((__le16 *)p);
}

static inline u32 get_unaligned_le32(const void *p)
{
	return le32_to_cpup((__le32 *)p);
}

static inline u64 get_unaligned_le64(const void *p)
{
	return le64_to_cpup((__le64 *)p);
}

static inline u16 get_unaligned_be16(const void *p)
{
	return be16_to_cpup((__be16 *)p);
}

static inline u32 get_unaligned_be32(const void *p)
{
	return be32_to_cpup((__be32 *)p);
}

static inline u64 get_unaligned_be64(const void *p)
{
	return be64_to_cpup((__be64 *)p);
}

static inline void put_unaligned_le16(u16 val, void *p)
{
	*((__le16 *)p) = cpu_to_le16(val);
}

static inline void put_unaligned_le32(u32 val, void *p)
{
	*((__le32 *)p) = cpu_to_le32(val);
}

static inline void put_unaligned_le64(u64 val, void *p)
{
	*((__le64 *)p) = cpu_to_le64(val);
}

static inline void put_unaligned_be16(u16 val, void *p)
{
	*((__be16 *)p) = cpu_to_be16(val);
}

static inline void put_unaligned_be32(u32 val, void *p)
{
	*((__be32 *)p) = cpu_to_be32(val);
}

static inline void put_unaligned_be64(u64 val, void *p)
{
	*((__be64 *)p) = cpu_to_be64(val);
}


/*
 * Cause a link-time error if we try an unaligned access other than
 * 1,2,4 or 8 bytes long
 */
extern void __bad_unaligned_access_size(void);

#define __get_unaligned_le(ptr) ((__force typeof(*(ptr)))({			\
	__builtin_choose_expr(sizeof(*(ptr)) == 1, *(ptr),			\
	__builtin_choose_expr(sizeof(*(ptr)) == 2, get_unaligned_le16((ptr)),	\
	__builtin_choose_expr(sizeof(*(ptr)) == 4, get_unaligned_le32((ptr)),	\
	__builtin_choose_expr(sizeof(*(ptr)) == 8, get_unaligned_le64((ptr)),	\
	__bad_unaligned_access_size()))));					\
	}))

#define __get_unaligned_be(ptr) ((__force typeof(*(ptr)))({			\
	__builtin_choose_expr(sizeof(*(ptr)) == 1, *(ptr),			\
	__builtin_choose_expr(sizeof(*(ptr)) == 2, get_unaligned_be16((ptr)),	\
	__builtin_choose_expr(sizeof(*(ptr)) == 4, get_unaligned_be32((ptr)),	\
	__builtin_choose_expr(sizeof(*(ptr)) == 8, get_unaligned_be64((ptr)),	\
	__bad_unaligned_access_size()))));					\
	}))

#define __put_unaligned_le(val, ptr) ({					\
	void *__gu_p = (ptr);						\
	switch (sizeof(*(ptr))) {					\
	case 1:								\
		*(u8 *)__gu_p = (__force u8)(val);			\
		break;							\
	case 2:								\
		put_unaligned_le16((__force u16)(val), __gu_p);		\
		break;							\
	case 4:								\
		put_unaligned_le32((__force u32)(val), __gu_p);		\
		break;							\
	case 8:								\
		put_unaligned_le64((__force u64)(val), __gu_p);		\
		break;							\
	default:							\
		__bad_unaligned_access_size();				\
		break;							\
	}								\
	(void)0; })

#define __put_unaligned_be(val, ptr) ({					\
	void *__gu_p = (ptr);						\
	switch (sizeof(*(ptr))) {					\
	case 1:								\
		*(u8 *)__gu_p = (__force u8)(val);			\
		break;							\
	case 2:								\
		put_unaligned_be16((__force u16)(val), __gu_p);		\
		break;							\
	case 4:								\
		put_unaligned_be32((__force u32)(val), __gu_p);		\
		break;							\
	case 8:								\
		put_unaligned_be64((__force u64)(val), __gu_p);		\
		break;							\
	default:							\
		__bad_unaligned_access_size();				\
		break;							\
	}								\
	(void)0; })

# define get_unaligned	__get_unaligned_le
# define put_unaligned	__put_unaligned_le

#endif

// vim:ts=4:sw=4

