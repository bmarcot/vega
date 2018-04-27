/*
 * kernel/fd.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/fdtable.h>
#include <kernel/kernel.h>
#include <kernel/mm/slab.h>

static struct kmem_cache *files_struct_cache;

struct files_struct *alloc_files_struct(void)
{
	struct files_struct *files;

	files = kmem_cache_alloc(files_struct_cache, CACHE_OPT_NONE);
	if (files)
		files->fdtab = 0;

	return files;
}

void put_files_struct(struct files_struct *files)
{
	kmem_cache_free(files_struct_cache, files);
}

int fd_init(void)
{
	files_struct_cache = KMEM_CACHE(files_struct);
	BUG_ON(!files_struct_cache);

	return 0;
}
