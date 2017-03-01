/*
 * kernel/bitops.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/param.h>

#include <kernel/bitops.h>
#include <kernel/kernel.h>

unsigned long ffsl(unsigned long word)
{
	return __builtin_ffsl(word);
}

unsigned long ffzl(unsigned long word)
{
	return __builtin_ffsl(~word);
}

unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
	for (unsigned long i = 0; i * BITS_PER_LONG < size; i++) {
		if (addr[i])
			return MIN(i * BITS_PER_LONG + ffsl(addr[i]) - 1, size);
	}

	return size;
}

unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	for (unsigned long i = 0; i * BITS_PER_LONG < size; i++) {
		if (addr[i] != ~0ul)
			return MIN(i * BITS_PER_LONG + ffzl(addr[i]) - 1, size);
	}

	return size;
}
