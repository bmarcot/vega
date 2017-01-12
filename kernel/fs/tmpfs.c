/*
 * kernel/fs/dev.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

/* static int tmpfs_ino = 0xbabe; */

int tmpfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	/* struct inode *inode; */

	/* inode = malloc(sizeof(struct inode) + sizeof(struct dentry)); */
	/* if (inode == NULL) */
	/* 	return -1; */
	/* inode->i_ino = tmpfs_ino++; */

	dir->i_mode = mode | S_IFDIR;
	dentry->d_inode = dir;
	INIT_LIST_HEAD(&dentry->d_subdirs);

	return 0;
}

int tmpfs_link(struct dentry *old_dentry, struct inode *dir, struct dentry *dentry)
{
	(void)old_dentry;

	dir->i_size++;
	list_add_tail(&dentry->d_child, &dentry->d_parent->d_subdirs);

	return 0;
}

int tmpfs_iterate(struct file *file, struct dir_context *ctx)
{
	int res = -1;
	struct dentry *dentry = file->f_dentry;

	if (file->f_pos == dentry->d_inode->i_size + 2)
		return -1;
	switch (file->f_pos) {
	case 0:
		res = dir_emit_dot(file, ctx);
		break;
	case 1:
		res = dir_emit_dotdot(file, ctx);
		break;
	default:;
		struct dentry *dent;
		int i = 2;
		list_for_each_entry(dent, &dentry->d_subdirs, d_child) {
			if (i++ == file->f_pos) {
				res = dir_emit(ctx, dent->d_name,
					strlen(dent->d_name), dent->d_inode->i_ino, 0);
				break;
			}
		}
	}
	file->f_pos++;

	return res;
}

struct dentry *tmpfs_lookup(struct inode *dir, struct dentry *target)
{
	//FIXME: entrie should be stored at dir->i_private, and dentries should
	//       be created dynamically
	(void)dir;

	struct dentry *dentry;

	list_for_each_entry(dentry, &target->d_parent->d_subdirs, d_child) {
		if (!strcmp(target->d_name, dentry->d_name)) {
			return dentry;
		}
	}

	return NULL;
}

const struct inode_operations tmpfs_iops = {
	.lookup = tmpfs_lookup,
};

const struct file_operations tmpfs_fops = {
	.iterate = tmpfs_iterate,
};

struct inode tmpfs_inodes[] = {
	{	/* /      - the root directory */
		.i_ino  = 1,
		.i_op   = &tmpfs_iops,
		.i_fop  = &tmpfs_fops,
	},
	{	/* /dev   - essential device files */
		.i_ino  = 2,
		.i_op   = &tmpfs_iops,
		.i_fop  = &tmpfs_fops,
	},
	{	/* /proc  - process and kernel information as files */
		.i_ino  = 3,
		.i_op   = &tmpfs_iops,
		.i_fop  = &tmpfs_fops,
	},
};

struct dentry tmpfs_dentries[] = {
	{	.d_inode  = &tmpfs_inodes[0],
		.d_parent = &tmpfs_dentries[0],
	},
	{	.d_inode  = &tmpfs_inodes[1],
		.d_parent = &tmpfs_dentries[0],
		.d_name   = "dev",
	},
	{	.d_inode  = &tmpfs_inodes[2],
		.d_parent = &tmpfs_dentries[0],
		.d_name   = "proc",
	},
};

void tmpfs_init(void)
{
	tmpfs_mkdir(&tmpfs_inodes[0], &tmpfs_dentries[0], 0);
	tmpfs_mkdir(&tmpfs_inodes[1], &tmpfs_dentries[1], 0);
	tmpfs_mkdir(&tmpfs_inodes[2], &tmpfs_dentries[2], 0);

	tmpfs_link(NULL, &tmpfs_inodes[0], &tmpfs_dentries[1]);
	tmpfs_link(NULL, &tmpfs_inodes[0], &tmpfs_dentries[2]);
}
