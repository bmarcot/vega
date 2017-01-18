/*
 * kernel/fs/vfs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

int vfs_iterate(struct file *file, struct dir_context *ctx)
{
	if (file->f_op->iterate == NULL)
		return -1;

	return file->f_op->iterate(file, ctx);
}

struct dentry *vfs_lookup(struct inode *dir, struct dentry *target)
{
	struct dentry *dentry;

	if (!S_ISDIR(dir->i_mode))
		return NULL;
	dentry = dir->i_op->lookup(dir, target);
	if (dentry)
		dentry->d_count++;

	return dentry;
}

int vfs_link(struct dentry *old_dentry, struct inode *dir, struct dentry *dentry)
{
	if (dir->i_op->link == NULL)
		return -1;

	return dir->i_op->link(old_dentry, dir, dentry);
}
