/*
 * include/kernel/mm.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _KERNEL_MM_H
#define _KERNEL_MM_H

#include <kernel/mm_types.h>
#include <kernel/types.h>

#define KMALLOC_MIN_SIZE 4

void *kmalloc(size_t size);
void *kzalloc(size_t size);
void kfree(void *ptr);

void mm_release(void);
struct mm_struct *alloc_mm_struct(void);
void put_mm_struct(struct mm_struct *mm);
int mm_init(void);

#endif /* !_KERNEL_MM_H */
