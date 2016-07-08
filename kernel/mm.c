#include <stdio.h>
#include <stdlib.h>

#include <kernel/mm.h>

#include "bitmap.h"
#include "utils.h"
#include "linux/list.h"
#include "kernel.h"

#ifdef DEBUG
#define DEBUG_PRINTK(...)					\
	do { printk(__VA_ARGS__); fflush(stdout); } while (0);
#else
#define DEBUG_PRINTK(...)
#endif /* DEBUG */

struct free_area free_area[MAX_BLOCK_ORDER + 1];

extern char __text_start__;
extern char __text_end__;
extern char __rodata_start__;
extern char __rodata_end__;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __pgmem_start__;
extern char __pgmem_end__;
extern char __pgmem_size__;
extern char __heap_start__;
extern char __heap_end__;

static inline unsigned addr_to_block_index(void *addr, unsigned order)
{
	//FIXME: multiple zone to allocate from? use the zone's base address instead
	return ((u32) addr - (u32) &__pgmem_start__) >> (order + MIN_PAGE_ORDER);
}

static inline int order_to_bytesz(unsigned order)
{
	return (1 << (order + MIN_PAGE_ORDER));
}

static unsigned bytesz_to_order(size_t size)
{
	unsigned order = 0;

	size = (size - 1) >> MIN_PAGE_ORDER;
	for (; order <= MAX_BLOCK_ORDER; order++, size >>= 1) {
		if (!size)
			break;
	}

	return order;
}

/* Split a block of order n into two  buddies of order n-1. The first block
   (lowest address) is inserted in the list of free blocks of order n-1, the
   second block (resp. highest address) is returned to the caller function.
   The caller function must explicitly allocate the block, or insert it into
   the list of free blocks of order n-1.    */
static void *split_block(void *block_addr, unsigned order)
{
	unsigned ix = addr_to_block_index(block_addr, order);
	void *new_block;

	bitmap_set(free_area[order].map, ix);
	bitmap_clear(free_area[order - 1].map, ix * 2);
	list_move(block_addr, &free_area[order - 1].free_list);
	new_block = block_addr + order_to_bytesz(order - 1);
	INIT_LIST_HEAD((struct list_head *) new_block);

	return new_block;
}

/* Try to allocate a block of order n. If no block for that order is available,
   then try to split a block of order n+1 into two buddies of order n. Call
   recursively until one block of higher order is found.    */
static void *try_alloc(unsigned order)
{
	void *page_addr;
	unsigned ix;

	if (order > MAX_BLOCK_ORDER) {
		printk("mm: alloc failed: no free block could be found\n");
		return NULL;
	}

	if (list_empty(&free_area[order].free_list)) {
		DEBUG_PRINTK("mm: no free block with order=%d, trying with order=%d\n",
			order, order + 1);
		page_addr = try_alloc(order + 1);
		if (page_addr != NULL)
			page_addr = split_block(page_addr, order + 1);
		else
			return NULL;
	} else {
		DEBUG_PRINTK("mm: found a free block with order=%d at %p\n",
			order, free_area[order].free_list.next);
		return free_area[order].free_list.next;
	}
	ix = addr_to_block_index(page_addr, order);
	bitmap_set(free_area[order].map, ix);

	return page_addr;
}

void *page_alloc(int size)
{
	void *page_addr;
	unsigned order;

	order = bytesz_to_order(size);
	if (list_empty(&free_area[order].free_list)) {
		/* slow path alloc */
		page_addr = try_alloc(order);
	} else {
		/* fast path alloc */
		page_addr = free_area[order].free_list.next;
		bitmap_set(free_area[order].map, addr_to_block_index(page_addr, order));
		list_del(page_addr);
	}

	DEBUG_PRINTK("mm: allocated a page at %p\n", page_addr);

	return page_addr;
}

/* .pgmem section is 32KiB:
 *    - 128 pages of 256B = a 16 bytes map
 *    -  64 pages of 512B = a 8 bytes map
 *    -  32 pages of 1KiB = a 4 bytes map
 *    -  16 pages of 2KiB = a 2 bytes map
 */
static u32 *const maps[] = { (u32 []){0, 0, 0, 0}, (u32 []){0, 0}, (u32 []){0}, (u32 []){0} };

extern char __early_stack_end__;

int page_init(void)
{
	void *block_addr = &__pgmem_start__;

	printk("Physical memory:\n");
	for (unsigned o = 0; o <= MAX_BLOCK_ORDER; o++) {
		INIT_LIST_HEAD(&free_area[o].free_list);
		free_area[o].page_count = (u32) &__pgmem_size__ / order_to_bytesz(o);
		printk("  - %3d pages of %4d Bytes\n", free_area[o].page_count,
			order_to_bytesz(o));
		free_area[o].map = maps[o];
	}

	/* Populate the high-order free list, but mark the last page as reserved.
	 * Last page in the pageable memory is reserved for the early stack used
	 * during kernel bootstrap. This page will be reclaimed by the system and
	 * released to the pageable pool just before jumping to the main thread. */
	for (unsigned i = 0; i < 15; i++) {
		list_add(block_addr, &free_area[MAX_BLOCK_ORDER].free_list);
		block_addr = block_addr + order_to_bytesz(MAX_BLOCK_ORDER);
	}
	bitmap_set(free_area[MAX_BLOCK_ORDER].map,
		addr_to_block_index(&__early_stack_end__, MAX_BLOCK_ORDER));

	printk("Memory map:\n");
	printk("  .text   = %08x--%08x  %6d Bytes\n", &__text_start__,
		&__text_end__, &__text_end__ - &__text_start__);
	printk("  .rodata = %08x--%08x  %6d Bytes\n", &__rodata_start__,
		&__rodata_end__, &__rodata_end__ - &__rodata_start__);
	printk("  .data   = %08x--%08x  %6d Bytes\n", &__data_start__,
		&__data_end__, &__data_end__ - &__data_start__);
	printk("  .bss    = %08x--%08x  %6d Bytes\n", &__bss_start__,
		&__bss_end__, &__bss_end__ - &__bss_start__);
	printk("  .heap   = %08x--%08x  %6d Bytes\n", &__heap_start__,
		&__heap_end__, &__heap_end__ - &__heap_start__);
	printk("  .pgmem  = %08x--%08x  %6d Bytes\n", &__pgmem_start__,
		&__pgmem_end__, &__pgmem_end__ - &__pgmem_start__);

	printk("Block bitmaps:");
	for (unsigned o = 0; o <= MAX_BLOCK_ORDER; o++)
		printk(" %d@%p", o, maps[o]);
	printk("\n");

	return 0;
}

static void *index_to_addr(unsigned ix, unsigned order)
{
	return &__pgmem_start__ + ix * order_to_bytesz(order);
}

static inline unsigned get_highest_order(void *ptr)
{
	unsigned order = 0;
	u32 addr = (u32) ptr >> MIN_PAGE_ORDER;

	for (; order < MAX_BLOCK_ORDER; order++, addr >>= 1) {
		if (addr & 1)
			break;
	}

	return order;
}

static unsigned get_buddy_index(unsigned ix)
{
	if (ix % 2)
		return ix - 1;
	return ix + 1;
}

static inline unsigned lower_buddy_index(unsigned ix)
{
	if (ix % 2)
		return ix - 1;
	return ix;
}

int try_coalesce(unsigned ix, unsigned order)
{
	if (order == MAX_BLOCK_ORDER)
		return 0;

	ix = lower_buddy_index(ix);
	if (bitmap_get(free_area[order].map, ix)
		|| bitmap_get(free_area[order].map, ix + 1))
		return 0;

	/* remove the blocks from the list of free blocks */
	list_del(index_to_addr(ix, order));
	list_del(index_to_addr(ix + 1, order));

	/* clear bit in the bitmap of upper order */
	bitmap_clear(free_area[order + 1].map, ix / 2);

	/* add a new block into the list of blocks of upper order */
	list_add(index_to_addr(ix / 2, order + 1),
		&free_area[order + 1].free_list);

	DEBUG_PRINTK("mm: coalesced two blocks of order=%d into one of order=%d\n",
		order, order + 1);

	return 1 + try_coalesce(ix / 2, order + 1);
}

void page_free(void *ptr)
{
	/* A pointer aligned on 2KiB can be any page between 2KiB and 256 bytes.
	 * Start from the highest posssible order and find the first bit set in
	 * the lowest order.    */
	int order = get_highest_order(ptr);
	for (int o = order; o >= 0; o--) {
		if (bitmap_get(free_area[o].map, addr_to_block_index(ptr, o)))
			order = o;
	}

	DEBUG_PRINTK("mm: free a page with order=%d\n", order);

	unsigned ix = addr_to_block_index(ptr, order);
	bitmap_clear(free_area[order].map, ix);
	if ((order < MAX_BLOCK_ORDER)
		&& !bitmap_get(free_area[order].map, get_buddy_index(ix))) {
#ifndef DEBUG
		try_coalesce(ix, order);
#else
		int n = try_coalesce(ix, order);
		DEBUG_PRINTK("mm: coalesced across %d orders\n", n);
#endif /* !DEBUG */
	} else {
		list_add(ptr, &free_area[order].free_list);
	}
}
