/*
 * kernel/mm/mm.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <errno.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <vega/sys/mman.h>

#include <kernel/mm/page.h>

#define M_ISANON(f)     (((f) & MAP_ANONYMOUS) == MAP_ANONYMOUS)
#define M_ISUNINIT(f)   (((f) & MAP_UNINITIALIZED) == MAP_UNINITIALIZED)

void *sys_mmap(void *addr, size_t length, __unused int prot,
	int flags, __unused int fd, __unused off_t offset)
{
	int order;

	if (!length) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	if (M_ISANON(flags)) {
		order = size_to_page_order(length);
		addr = alloc_pages(order);
	} else {
		//FIXME: Support file mapping --baruch
		return MAP_FAILED;
	}
	if (addr == NULL) {
		errno = ENOMEM;
		return MAP_FAILED;
	}
	if (M_ISANON(flags) & !M_ISUNINIT(flags))
		memset(addr, 0, length);

	return addr;
}
