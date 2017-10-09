/*
 * include/kernel/fs/tmpfs.h
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#ifndef _KERNEL_FS_TMPFS_H
#define _KERNEL_FS_TMPFS_H

#include <kernel/fs.h>

int tmpfs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
		int /* bool */ exclusive);
int tmpfs_mkdir(struct inode *dir, struct dentry *dentry, int mode);
int tmpfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode,
		dev_t dev);

#endif /* !_KERNEL_FS_TMPFS_H */
