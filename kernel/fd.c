/*
 * kernel/fd.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/fdtable.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/mm/slab.h>

static struct kmem_cache *files_struct_cache;
static struct kmem_cache *file_cache;

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

struct file *alloc_file(void)
{
	return kmem_cache_alloc(file_cache, CACHE_OPT_NONE);
}

void put_file(struct file *file)
{
	kmem_cache_free(file_cache, file);
}

int fd_init(void)
{
	files_struct_cache = KMEM_CACHE(files_struct);
	BUG_ON(!files_struct_cache);

	file_cache = KMEM_CACHE(file);
	BUG_ON(!file_cache);

	return 0;
}
