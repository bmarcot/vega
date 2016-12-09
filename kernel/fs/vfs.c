/*
 * kernel/fs/vfs.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

int vfs_iterate(struct file *file, struct dir_context *ctx)
{
	if (file->f_op->iterate == NULL)
		return -1; // -ENOTDIR

	return file->f_op->iterate(file, ctx);
}

struct dentry *vfs_lookup(struct inode *dir, struct dentry *target)
{
	struct dentry *dentry;

	if (!S_ISDIR(dir->i_mode)) {
		printk("Not a dir\n");
		return NULL;
	}
	dentry = dir->i_op->lookup(dir, target);
	if (dentry)
		dentry->d_count++;

	return dentry;
}
