#include <stdint.h>

#include <stddef.h>
#include <stdio.h>

#define PAGE_SIZE 1024

extern intptr_t pgpool_base, pgpool_limit;

static int32_t alloc_bitmap = 0;    /* 32 pages of 1kB */

void *page_alloc(void)
{
    if (alloc_bitmap == ~0)
	return NULL;
    uint32_t i = 0, bm = alloc_bitmap;
    for (; bm && (bm & 1); bm >>= 1, i++)
	;
    alloc_bitmap |= (1 << i);

    printf("(debug) alloc %dB at %p from .pgpool\n", PAGE_SIZE, (void *) &pgpool_base + i * PAGE_SIZE);

    return (void *) &pgpool_base + i * PAGE_SIZE;
}
