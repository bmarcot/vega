/*
 * include/kernel/kernel.h
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <kernel/compiler.h>
#include <kernel/printk.h>
#include <kernel/stddef.h>

/* round-down to a power of 2 */
#define align(x, a)          align_mask(x, (__typeof__(x))((a) - 1))
#define align_mask(x, mask)  ((x) & ~(mask))

/* round-up to a power of 2 */
#define align_next(x, a)          align_next_mask(x, (__typeof__(x))((a) - 1))
#define align_next_mask(x, mask)  (((x) + (mask)) & ~(mask))

#define ARRAY_SIZE(arr) \
	((int)(sizeof(arr) / sizeof(*(arr))))

#define ARRAY_INDEX(elt, arr)				\
	({ unsigned int _elt = (unsigned int)(elt);	\
	   unsigned int _arr = (unsigned int)(arr);	\
	   (_elt - _arr) / sizeof(*(elt)); })

/*
 * Endianess conversion
 */

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ /* GNU C extension */
#  include "platform.h"
#  define le16_to_cpu(val)  (val)
#  define le32_to_cpu(val)  (val)
#  define be16_to_cpu(val)  __REV16((val))
#  define be32_to_cpu(val)  __REV((val))
#endif

/*
 * Bug!
 */

static inline __attribute__((noreturn)) void panic(const char *fmt, ...)
{
	printk(fmt);
	for (;;)
		;
}

#define BUG()								\
	do {								\
		printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
		panic("BUG!");						\
	} while (0)

#define BUG_ON(condition)					\
	do {							\
		if (unlikely(condition)) BUG();			\
	} while(0)

#define BUILD_BUG_ON(condition) do { _Static_assert(condition, "BUILD BUG"); } while (1);

#define BUILD_BUG() BUILD_BUG_ON(1)

#endif /* !_KERNEL_KERNEL_H */
