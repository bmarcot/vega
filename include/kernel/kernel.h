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

#define ARRAY_SIZE(arr) \
	((size_t)(sizeof(arr) / sizeof(*(arr))))

#define ARRAY_INDEX(elt, arr)				\
	({ unsigned int _elt = (unsigned int)(elt);	\
	   unsigned int _arr = (unsigned int)(arr);	\
	   (_elt - _arr) / sizeof(*(elt)); })

#define container_of(ptr, type, member) ({				\
	    const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	    (type *)( (char *)__mptr - offsetof(type,member) );})

int printk(const char *fmt, ...);

#define pr_fmt(fmt, ...)				   \
	do {						   \
		printk(fmt, __func__, ##__VA_ARGS__);	   \
	} while (0)

#define make_fmt(type, fmt) type": %s: "fmt"\n"

#define pr_err(fmt, ...) pr_fmt(make_fmt("error", fmt), ##__VA_ARGS__)
#define pr_warn(fmt, ...) pr_fmt(make_fmt("warning", fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...) pr_fmt(make_fmt("info", fmt), ##__VA_ARGS__)

#endif /* !_KERNEL_KERNEL_H */
