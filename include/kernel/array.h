/*
 * include/kernel/array.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_ARRAY_H
#define _KERNEL_ARRAY_H

/* Iterate over an array of elements where each element can use
 * more storage than the size of its type. */
#define array_for_each_element(pos, first, num, size)		\
	for (pos = (__typeof__(pos))(first);			\
	     (char *)pos < (char *)(first) + (num) * (size);	\
	     pos = (__typeof__(pos))((char *)pos + (size)))

#endif /* !_KERNEL_ARRAY_H */
