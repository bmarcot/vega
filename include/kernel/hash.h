/*
 * include/kernel/hash.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_HASH_H
#define _KERNEL_HASH_H

#include <stddef.h>

unsigned long hash_djb2(unsigned char *str, size_t len);
unsigned long hash_sdbm(unsigned char *str, size_t len);

#endif /* _KERNEL_HASH_H */
