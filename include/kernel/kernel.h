/*
 * include/kernel/kernel.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

/* round-down to a power of 2 */
#define align(x, a)          align_mask(x, (__typeof__(x))((a) - 1))
#define align_mask(x, mask)  ((x) & ~(mask))

/* round-up to a power of 2 */
#define align_next(x, a)          align_next_mask(x, (__typeof__(x))((a) - 1))
#define align_next_mask(x, mask)  (((x) + (mask)) & ~(mask))

#define max(a, b)			\
	({ __typeof__(a) _a = (a);	\
	   __typeof__(b) _b = (b);	\
	   _a > _b ? _a : _b; })

#define min(a, b)			\
	({ __typeof__(a) _a = (a);	\
	   __typeof__(b) _b = (b);	\
	   _a < _b ? _a : _b; })

#define ARRAY_SIZE(arr) \
	(sizeof(arr) / sizeof(*(arr)))

#define ARRAY_INDEX(elt, arr)				\
	({ unsigned int _elt = (unsigned int)(elt);	\
	   unsigned int _arr = (unsigned int)(arr);	\
	   (_elt - _arr) / sizeof(*(elt)); })

#define container_of(ptr, type, member) ({				\
	    const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	    (type *)( (char *)__mptr - offsetof(type,member) );})

int printk(const char *fmt, ...);

#endif /* !_KERNEL_KERNEL_H */
