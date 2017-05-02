/*
 * system/kmastat.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <string.h>
#include <kernel/kernel.h>
#include <kernel/mm/slab.h>
#include "linux/list.h"

extern struct list_head caches;

static void pad(int len)
{
	for (int i = 0; i < len; i++)
		printk(" ");
}

int kmastat(__unused int argc, __unused char *argv[])
{
	struct kmem_cache *cache;
	struct slab *slab;
	int slab_count, buf_in_use, buf_total;

	printk("cache                        buf    buf    buf    memory     alloc alloc\n");
	printk("name                        size in use  total    in use   succeed  fail\n");
	printk("------------------------- ------ ------ ------ --------- --------- -----\n");

	list_for_each_entry(cache, &caches, list) {
		slab_count = 0;
		buf_in_use = 0;
		buf_total = 0;

		list_for_each_entry(slab, &cache->slabs_free, list) {
			slab_count++;
			buf_total += cache->objnum;
		}
		list_for_each_entry(slab, &cache->slabs_partial, list) {
			slab_count++;
			buf_total += cache->objnum;
			buf_in_use += cache->objnum - slab->free_objects;
		}
		list_for_each_entry(slab, &cache->slabs_full, list) {
			slab_count++;
			buf_total += cache->objnum;
			buf_in_use += cache->objnum;
		}

		/* cache name */
		printk("%s", cache->name);
		int len = strlen(cache->name);
		pad(26 - len);

		/* buf size */
		printk("% 6d ", cache->objsize);

		/* buf in use */
		printk("% 6d ", buf_in_use);

		/* buf total */
		printk("% 6d ", buf_total);

		/* memory in use */
		printk("% 9d ", slab_count * CACHE_PAGE_SIZE);

		/* alloc succeed */
		printk("% 9d ", cache->alloc_succeed);

		/* alloc fail */
		printk("% 5d\n", cache->alloc_fail);
	}
	printk("\n");

	return 0;
}
