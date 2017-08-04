/*
 * kernel/mm/kmalloc.c
 *
 * Copyright (c) 2016-2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/types.h>

/* This is a boundary tag, located at the beginning of each allocated
 * and free blocks of memory.    */
struct malloc_tag {
	struct {
		u32      is_free : 1;
		u32      length  : 31;
	};
	struct list_head list;
	unsigned char    data[0];
} __attribute__((packed));

static LIST_HEAD(blocks);

void kernel_heap_init(void *heap_start, size_t heap_size)
{
	struct malloc_tag *first_block = heap_start;

	first_block->is_free = 1;
	first_block->length = heap_size;
	list_add(&first_block->list, &blocks);
}

void *kmalloc(size_t size)
{
	struct malloc_tag *block, *new_block;

	 /* word-aligned */
	size = align_next(size, KMALLOC_MIN_SIZE) + sizeof(struct malloc_tag);

	/* first-fit search */
	list_for_each_entry(block, &blocks, list) {
		if (block->is_free && (block->length >= size)) {
			if ((block->length - size) > sizeof(struct malloc_tag)) {
				block->length -= size;
				new_block = (struct malloc_tag *)((u32)block + block->length);
				new_block->is_free = 0;
				new_block->length = size;
				list_add(&new_block->list, &block->list);
				return new_block->data;
			} else {
				block->is_free = 0;
				return block->data;
			}
		}
	}

	return NULL;
}

void kfree(void *ptr)
{
	struct malloc_tag *block = container_of(ptr, struct malloc_tag, data);

	block->is_free = 1;

	/* merge with a free previous block */
	if (!list_is_first(&block->list, &blocks)) {
		struct malloc_tag *prev_block = list_prev_entry(block, list);
		if (prev_block->is_free) {
			prev_block->length += block->length;
			list_del(&block->list);
			block = prev_block;
		}
	}

	/* merge with a free next block */
	if (!list_is_last(&block->list, &blocks)) {
		struct malloc_tag *next_block = list_next_entry(block, list);
		if (next_block->is_free) {
			block->length += next_block->length;
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
