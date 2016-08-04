/*
 * kernel/bitops.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <limits.h>

#include <kernel/bitops.h>
#include "kernel.h"

unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
	for (int i = 0; i * sizeof(unsigned long) * CHAR_BIT < size; i++) {
		if (addr[i])
			return min(i * sizeof(unsigned long) * CHAR_BIT
				+ ffs(addr[i]), size);
	}

	return size;
}

unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	for (int i = 0; i * sizeof(unsigned long) * CHAR_BIT < size; i++) {
		if (addr[i] != ~((unsigned long)0))
			return min(i * sizeof(unsigned long) * CHAR_BIT
				+ ffz(addr[i]), size);
	}

	return size;
}
