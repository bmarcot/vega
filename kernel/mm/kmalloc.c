/*
 * kernel/mm/kmalloc.c
 *
 * Copyright (c) 2016-2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/types.h>

/* This is a boundary tag, located at the beginning of each allocated
 * and free blocks of memory.    */
struct malloc_tag {
	/* composite field:
	 *   -  bit 31       0 = allocated block, 1 = free block
	 *   -  bit 30..0    block length (including this tag)
	 */
	u32              free__length;
	struct list_head list;
	unsigned char    data[0];
};

static LIST_HEAD(blocks);

#define BLOCK_FREE_MASK   0x80000000
#define BLOCK_LENGTH_MASK 0x7fffffff

static inline size_t get_blk_length(struct malloc_tag *block)
{
	return block->free__length & BLOCK_LENGTH_MASK;
}

static inline void set_blk_length(struct malloc_tag *block, size_t len)
{
	block->free__length = (block->free__length & BLOCK_FREE_MASK) | len;
}

static inline int is_blk_free(struct malloc_tag *block)
{
	return !!(block->free__length & BLOCK_FREE_MASK);
}

static inline void alloc_blk(struct malloc_tag *block)
{
	block->free__length &= BLOCK_LENGTH_MASK;
}

static inline void free_blk(struct malloc_tag *block)
{
	block->free__length |= BLOCK_FREE_MASK;
}

void kernel_heap_init(void *heap_start, size_t heap_size)
{
	struct malloc_tag *first_block = heap_start;

	first_block->free__length = BLOCK_FREE_MASK | heap_size;
	list_add(&first_block->list, &blocks);
}

void *kmalloc(size_t size)
{
	struct malloc_tag *free_blk, *new_block;

	 /* allocation size is a multiple of 4-byte aligned, plus size of tag */
	size = align_next(size, 4) + sizeof(struct malloc_tag);

	/* find a free block wich is large enough to fullfill the memory requirement */
	list_for_each_entry(free_blk, &blocks, list) {
		if (is_blk_free(free_blk) && (get_blk_length(free_blk) >= size)) {
			if ((get_blk_length(free_blk) - size) > sizeof(struct malloc_tag)) {
				set_blk_length(free_blk, get_blk_length(free_blk) - size);
				new_block = (struct malloc_tag *)((u32)free_blk
								+ get_blk_length(free_blk));
				alloc_blk(new_block);
				set_blk_length(new_block, size);
				list_add(&new_block->list, &free_blk->list);
				return new_block->data;
			} else {
				alloc_blk(free_blk);
				return free_blk->data;
			}
		}
	}

	return NULL;
}

void kfree(void *ptr)
{
	struct malloc_tag *block = container_of(ptr, struct malloc_tag, data);
	struct malloc_tag *prev_block, *next_block;

	free_blk(block);

	/* merge with previous block if free */
	if (block->list.prev != &blocks) {
		prev_block = list_prev_entry(block, list);
		if (is_blk_free(prev_block)) {
			set_blk_length(prev_block, get_blk_length(prev_block)
					+ get_blk_length(block));
			list_del(&block->list);
			block = prev_block;
		}
	}

	/* merge with next block if free */
	if (block->list.next != &blocks) {
		next_block = list_next_entry(block, list);
		if (is_blk_free(next_block)) {
			set_blk_length(block, get_blk_length(block)
				+ get_blk_length(next_block));
			list_del(&next_block->list);
		}
	}
}

void *malloc(size_t size)
{
	return kmalloc(size);
}

void free(void *ptr)
{
	return kfree(ptr);
}
