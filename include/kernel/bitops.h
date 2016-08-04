/*
 * include/kernel/bitops.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_BITOPS_H
#define KERNEL_BITOPS_H

static inline unsigned long ffs(unsigned long word)
{
	return __builtin_clz(word);
}

static inline unsigned long ffz(unsigned long word)
{
	return __builtin_clz(~word);
}

unsigned long find_first_bit(const unsigned long *addr, unsigned long size);
unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size);

#endif /* !KERNEL_BITOPS_H */
