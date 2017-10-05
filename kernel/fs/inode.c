/*
 * kernel/fs/inode.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/fs.h>

extern const struct file_operations def_chr_fops;

void init_special_inode(struct inode *inode, umode_t mode, kdev_t rdev)
{
	inode->i_mode = mode;
	if (S_ISCHR(mode)) {
		inode->i_fop = &def_chr_fops;
		inode->i_rdev = rdev;
	}
}
