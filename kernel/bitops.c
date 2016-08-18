/*
 * kernel/bitops.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <kernel/bitops.h>
#include "kernel.h"

unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
	for (unsigned long i = 0; i * BITS_PER_LONG < size; i++) {
		if (addr[i])
			return min(i * BITS_PER_LONG + ffs(addr[i]), size);
	}

	return size;
}

unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	for (unsigned long i = 0; i * BITS_PER_LONG < size; i++) {
		if (addr[i] != ~0ul)
			return min(i * BITS_PER_LONG + ffz(addr[i]), size);
	}

	return size;
}
