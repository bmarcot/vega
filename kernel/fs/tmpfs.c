/*
 * kernel/fs/dev.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

struct tmpfs_dirent {
	struct inode     *inode;
	char             name[NAME_MAX];
	struct list_head list;
};

int tmpfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	/* struct inode *inode; */

	/* inode = malloc(sizeof(struct inode) + sizeof(struct dentry)); */
	/* if (inode == NULL) */
	/* 	return -1; */
	/* inode->i_ino = tmpfs_ino++; */

	dir->i_mode = mode | S_IFDIR;
	dentry->d_inode = dir;
	/* INIT_LIST_HEAD(&dentry->d_subdirs); */

	struct list_head *dirlist = (struct list_head *)dir->i_private;
	INIT_LIST_HEAD(dirlist);

	return 0;
}

int tmpfs_link(struct dentry *old_dentry, struct inode *dir, struct dentry *dentry)
{
	(void)old_dentry;

	struct list_head *dirlist;
	struct tmpfs_dirent *new;

	new = malloc(sizeof(struct tmpfs_dirent));
	if (new == NULL)
		return -1;
	new->inode = dentry->d_inode;
	strncpy(new->name, dentry->d_name, NAME_MAX);
	dirlist = (struct list_head *)dir->i_private;
	list_add_tail(&new->list, dirlist);
	dir->i_size++;

	return 0;
}

int tmpfs_iterate(struct file *file, struct dir_context *ctx)
{
	int res = -1;
	struct inode *inode = file->f_dentry->d_inode;

	if (file->f_pos == inode->i_size + 2)
		return -1;
	switch (file->f_pos) {
	case 0:
		res = dir_emit_dot(file, ctx);
		break;
	case 1:
		res = dir_emit_dotdot(file, ctx);
		break;
	default:;
		struct tmpfs_dirent *dirent;
		struct list_head *dirlist = (struct list_head *)inode->i_private;
		int i = 2;
		list_for_each_entry(dirent, dirlist, list) {
			if (i++ == file->f_pos) {
				res = dir_emit(ctx, dirent->name,
					strlen(dirent->name), dirent->inode->i_ino, 0);
				break;
			}
		}
	}
	file->f_pos++;

	return res;
}

struct dentry *tmpfs_lookup(struct inode *dir, struct dentry *target)
{
	struct list_head *dirlist = (struct list_head *)dir->i_private;
	struct tmpfs_dirent *dirent;

	list_for_each_entry(dirent, dirlist, list) {
		if (!strcmp(target->d_name, dirent->name)) {
			target->d_inode = dirent->inode;
			return target;
		}
	}

	return NULL;
}

const struct inode_operations tmpfs_iops = {
	.lookup = tmpfs_lookup,
	.link   = tmpfs_link,
};

const struct file_operations tmpfs_fops = {
	.iterate = tmpfs_iterate,
};

static struct inode tmpfs_inodes[] = {
	{	/* /      - the root directory */
		.i_ino     = 1,
		.i_op      = &tmpfs_iops,
		.i_fop     = &tmpfs_fops,
		.i_mode    = S_IFDIR,
		.i_size    = 2,
		.i_private = &((struct list_head){ .next = 0, .prev = 0 }),
	},
	{	/* /dev   - essential device files */
		.i_ino     = 2,
		.i_op      = &tmpfs_iops,
		.i_fop     = &tmpfs_fops,
		.i_mode    = S_IFDIR,
		.i_private = &((struct list_head){ .next = 0, .prev = 0 }),
	},
	{	/* /proc  - process and kernel information as files */
		.i_ino     = 3,
		.i_op      = &tmpfs_iops,
		.i_fop     = &tmpfs_fops,
		.i_mode    = S_IFDIR,
		.i_private = &((struct list_head){ .next = 0, .prev = 0 }),
	},
};

struct inode *root_inode(void)
{
	return &tmpfs_inodes[0];
}

struct inode *dev_inode(void)
{
	return &tmpfs_inodes[1];
}

struct dentry *root_dentry(void)
{
	static struct dentry dentry = {
		.d_inode  = &tmpfs_inodes[0],
		.d_parent = &dentry,
	};

	return &dentry;
}

void tmpfs_init(void)
{
	static struct tmpfs_dirent dirents[] = {
		{ .inode = &tmpfs_inodes[1], .name = "dev",  },
		{ .inode = &tmpfs_inodes[2], .name = "proc", },
	};

	struct list_head *rootdir = (struct list_head *)tmpfs_inodes[0].i_private;
	INIT_LIST_HEAD(rootdir);
	list_add_tail(&dirents[0].list, rootdir);
	list_add_tail(&dirents[1].list, rootdir);

	struct list_head *devdir = (struct list_head *)tmpfs_inodes[1].i_private;
	INIT_LIST_HEAD(devdir);
}
