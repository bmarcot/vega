/*
 * include/kernel/page.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_PAGE_H
#define KERNEL_PAGE_H

#define MAX_PAGE_ORDER  3    /* max page size is 2 KiB */
#define MIN_PAGE_SIZE   256  /* min page size is 256 bytes */

void *alloc_pages(unsigned long order);
void free_pages(unsigned long addr, unsigned long order);
void show_page_bitmap(void);
long size_to_page_order(unsigned long size);

#endif /* !KERNEL_PAGE_H */
