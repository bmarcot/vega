//FIXME: for debug-purpose
#include <stddef.h>
#include <stdio.h>

#include "page.h"
#include "linux/types.h"

extern void *pgpool_base, *pgpool_limit;
static u32 alloc_bitmap = 0;    /* 32 pages of PAGE_SIZE kB */

void *page_alloc(void)
{
	/* check if the registering bitmap is full */
	if (alloc_bitmap == (u32) ~0)
		return NULL;

	u32 i = 0, bm = alloc_bitmap;
	for (; bm && (bm & 1); bm >>= 1, i++)
		;
	alloc_bitmap |= (1 << i);

	printf("(debug) alloc %dB at %p from .pgpool\n", PAGE_SIZE,
		(void *) &pgpool_base + i * PAGE_SIZE);

	return (void *) &pgpool_base + i * PAGE_SIZE;
}
