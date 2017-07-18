/*
 * kernel/fs/vfs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <kernel/fs.h>

int vfs_iterate(struct file *file, struct dir_context *ctx)
{
	if (file->f_op->iterate == NULL)
		return -1;

	return file->f_op->iterate(file, ctx);
}

struct dentry *vfs_lookup(struct inode *dir, struct dentry *target)
{
	if (dir->i_op->lookup == NULL)
		return NULL;
	if (!S_ISDIR(dir->i_mode))
		return NULL;

	return dir->i_op->lookup(dir, target);
}

int vfs_link(struct dentry *old_dentry, struct inode *dir, struct dentry *dentry)
{
	if (dir->i_op->link == NULL)
		return -1;

	return dir->i_op->link(old_dentry, dir, dentry);
}

int vfs_delete(struct dentry *dentry)
{
	if (dentry->d_op->delete == NULL)
		return -1;

	return dentry->d_op->delete(dentry);
}

void vfs_release(struct dentry *dentry)
{
	if (dentry->d_op->release)
		return dentry->d_op->release(dentry);
}

int vfs_mmap(struct file *file, off_t offset, void **addr)
{
	if (file->f_op->mmap == NULL)
		return -1;

	return file->f_op->mmap(file, offset, addr);
}
