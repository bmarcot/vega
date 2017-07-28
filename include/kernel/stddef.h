/*
 * include/kernel/stddef.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_STDDEF_H
#define _KERNEL_STDDEF_H

#include <kernel/compiler.h>

#undef  NULL
#define NULL ((void *)0)

#define container_of(ptr, type, member) ({				\
	    const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	    (type *)( (char *)__mptr - offsetof(type,member) );})

#endif /* !_KERNEL_STDDEF_H */
