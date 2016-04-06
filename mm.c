#include <stdio.h>
#include <stdlib.h>
#include "mm.h"
#include "bitmap.h"
#include "utils.h"
#include "linux/list.h"
#include "kernel.h"

#define DEBUG_PRINTK(...)\
	do { printk(__VA_ARGS__); fflush(stdout); } while (0);

struct free_area free_area[MAX_BLOCK_ORDER + 1];

extern void *__pgmem_base__;
extern void *__pgmem_size__;

static inline unsigned get_block_ix(void *addr, unsigned order)
{
	//FIXME: multiple zone to allocate from? use the zone's base address instead
	return ((u32) addr - (u32) &__pgmem_base__) >> (order + MIN_PAGE_ORDER);
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
	unsigned ix = get_block_ix(block_addr, order);
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
		DEBUG_PRINTK("mm: alloc failed: no free block could be found\n");
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
	ix = get_block_ix(page_addr, order);
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
		bitmap_set(free_area[order].map, get_block_ix(page_addr, order));
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
static u32 *maps[] = { (u32 []){0, 0, 0, 0}, (u32 []){0, 0}, (u32 []){0}, (u32 []){0} };

int page_init(void)
{
	void *block_addr = &__pgmem_base__;

	printk("physical memory splitting:\n");
	for (unsigned o = 0; o <= MAX_BLOCK_ORDER; o++) {
		INIT_LIST_HEAD(&free_area[o].free_list);
		free_area[o].page_count = (u32) &__pgmem_size__ / order_to_bytesz(o);
		printk("  - %3d pages of %4d bytes\n", free_area[o].page_count,
			order_to_bytesz(o));
		free_area[o].map = maps[o];
	}

	/* populate the high-order free list */
	for (unsigned i = 0; i < 16; i++) {
		list_add(block_addr, &free_area[MAX_BLOCK_ORDER].free_list);
		block_addr = block_addr + order_to_bytesz(MAX_BLOCK_ORDER);
	}

	printk("__pgmem_base__ is set to %p\n", &__pgmem_base__);
	printk("__pgmem_size__ is set to %p\n", &__pgmem_size__);

	return 0;
}
