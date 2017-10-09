/*
 * kernel/fs/inode.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/fs.h>
#include <kernel/kernel.h>

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

ino_t alloc_inode(void)
{
	static ino_t ino = 10000;

	return ino++;
}
