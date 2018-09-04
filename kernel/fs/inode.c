/*
 * kernel/fs/inode.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm/slab.h>

static struct kmem_cache *inode_cache;
extern const struct file_operations def_chr_fops;

void init_special_inode(struct inode *inode, umode_t mode, kdev_t rdev)
{
	inode->i_mode = mode;
	if (S_ISCHR(mode)) {
		inode->i_fop = &def_chr_fops;
		inode->i_rdev = rdev;
	} else {
		pr_err("Unknown filemode (%o) for inode %d", mode,
			inode->i_ino);
	}
}

static struct inode *__alloc_inode(struct super_block *sb)
{
	struct inode *inode;

	if (sb->s_op->alloc_inode)
		inode = sb->s_op->alloc_inode(sb);
	else
		inode = kmem_cache_alloc(inode_cache, CACHE_OPT_NONE);
	if (inode) {
		inode->i_mode = 0;
		inode->i_ino = -1;
		inode->i_count = 0;
		inode->i_size = 0;
		inode->i_op = NULL;
		inode->i_fop = NULL;
		inode->i_sb = sb;
		inode->i_dentry = NULL;
		inode->i_private = NULL;
	}

	return inode;
}

void put_inode(struct inode *inode)
{
	if (inode->i_sb->s_op->destroy_inode)
		inode->i_sb->s_op->destroy_inode(inode);
	else
		kmem_cache_free(inode_cache, inode);
}

struct inode *new_inode(struct super_block *sb)
{
	struct inode *inode;
	static ino_t last_ino = 10000;

	inode = __alloc_inode(sb);
	if (inode) {
		list_add(&inode->i_list, &sb->s_inodes);
		inode->i_ino = ++last_ino;
	}

	return inode;
}

void inode_init(void)
{
	inode_cache = KMEM_CACHE(inode);
	BUG_ON(!inode_cache);
}
