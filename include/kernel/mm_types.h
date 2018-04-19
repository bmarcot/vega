/*
 * include/kernel/mm_types.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_MM_TYPES_H
#define _KERNEL_MM_TYPES_H

#include <kernel/list.h>

struct mm_region {
	void			*start;
	unsigned long		length;
	int			flags;
	struct list_head	list;
};

struct mm_struct {
	unsigned long		size;
	unsigned long		max_size;
	struct list_head	region_head;
	int			refcount;
};

#endif /* !_KERNEL_MM_TYPES_H */
