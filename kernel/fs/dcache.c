/*
 * kernel/dcache.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm/slab.h>
#include <kernel/string.h>

static struct kmem_cache *dentry_cache;

//FIXME: Not parent but superblock
struct dentry *__d_alloc(struct dentry *parent, const char *name)
{
	struct dentry *dentry;

	dentry = kmem_cache_alloc(dentry_cache, CACHE_OPT_NONE);
	if (!dentry)
		return NULL;

	dentry->d_count = 0;
	dentry->d_inode = NULL;
	dentry->d_op = NULL; //FIXME: Must set from sb->s_dops
	dentry->d_parent = dentry;
	INIT_LIST_HEAD(&dentry->d_subdirs);

	return dentry;
}

struct dentry *d_alloc(struct dentry *parent, const char *name)
{
	struct dentry *dentry;

	dentry = __d_alloc(parent, name);
	if (!dentry)
		return NULL;

	strncpy(dentry->d_name, name, NAME_MAX);
	dentry->d_parent = parent;
	list_add(&dentry->d_child, &parent->d_subdirs);

	return dentry;
}

void d_put(struct dentry *dentry)
{
	list_del(&dentry->d_child);
	kmem_cache_free(dentry_cache, dentry);
}

void d_instantiate(struct dentry *dentry, struct inode *inode)
{
	if (inode) {
		dentry->d_inode = inode;
		inode->i_dentry = dentry;
	}
}

struct dentry *d_lookup(const struct dentry *parent, const char *name)
{
	struct dentry *dentry;

	list_for_each_entry(dentry, &parent->d_subdirs, d_child) {
		if (!strcmp(dentry->d_name, name))
			return dentry;
	}

	return NULL;
}

int dcache_init(void)
{
	dentry_cache = KMEM_CACHE(dentry);
	BUG_ON(!dentry_cache);

	return 0;
}
