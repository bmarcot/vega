/*
 * include/kernel/stdlib.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_STDLIB_H
#define _KERNEL_STDLIB_H

#include <kernel/types.h>

void *kmalloc(size_t size);
void kfree(void *ptr);

#endif /* !_KERNEL_STDLIB_H */
