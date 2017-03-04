/*
 * kernel/mm/mm.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <sys/types.h>

#include <kernel/mm/page.h>

void *sys_mmap(void *addr, size_t length, int prot, int flags,
	int fd, off_t offset)
{
	(void)addr, (void)prot, (void)flags, (void)fd, (void)offset;

	int order = size_to_page_order(length);

	return alloc_pages(order);
}
