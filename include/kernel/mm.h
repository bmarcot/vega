/*
 * include/kernel/mm.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_MM_H
#define _KERNEL_MM_H

#include <kernel/types.h>

void *kmalloc(size_t size);
void kfree(void *ptr);

#endif /* !_KERNEL_MM_H */
