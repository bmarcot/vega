/*
 * include/kernel/mm/slab.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _KERNEL_MM_SLAB_H
#define _KERNEL_MM_SLAB_H

#include <kernel/stddef.h>
#include <kernel/types.h>

#define CACHE_PAGE_SIZE	256
#define CACHE_NAMELEN	16

#define CACHE_OPT_NONE	0
/* OPT_FORCE: create a new cache, even when the object size matches a generic
 *            cache (i.e. 8B, 16B, 32B...)
 * OPT_PERSIST: this cache cannot be destroyed (i.e. when a cache w/o OPT_FORCE
 *              has been created and silently merged into a general object
 *              cache and the user try to destroy the cache he created)
 */

struct kmem_cache {
	struct list_head	slabs_free;
	struct list_head	slabs_partial;
	struct list_head	slabs_full;
	struct list_head	list;		/* linked list of caches */
	unsigned short		objsize;	/* size of one object within a slab */
	unsigned short		objnum;		/* number of objects per slab */
	int			opts;
	int			alloc_succeed;
	int			alloc_fail;
	char			name[CACHE_NAMELEN];
};

struct slab {
	unsigned long		free_bitmap[1];
	int			free_objects;	/* number of free objects in that slab */
	struct list_head	list;		/* pointer to prev/next slabs for that cache */
	char			data[0];
	/* struct kmem_cache *backlink; // backlink to the cache structure */
};

#define KMEM_CACHE(__struct)					\
	kmem_cache_create(#__struct, sizeof(struct __struct),	\
			0, CACHE_OPT_NONE, NULL)

struct kmem_cache *kmem_cache_create(const char *name, size_t size,
				size_t align, unsigned long flags,
				void (*ctor)(void *));
void *kmem_cache_alloc(struct kmem_cache *cache, unsigned long flags);
void kmem_cache_free(struct kmem_cache *cache, void *obj);
void kmem_cache_print_info(const struct kmem_cache *cache);
void kmem_cache_init(void);

#endif /* _KERNEL_MM_SLAB_H */
