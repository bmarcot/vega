/*
 * kernel/fs/tmpfs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/list.h>

struct __dirent {
	struct inode     *inode;
	char             name[NAME_MAX];
	struct list_head list;
};

#define TMPFS_DIRLIST(dir) \
	((struct list_head *)(dir)->i_private)

static struct __dirent *alloc_dirent(void)
{
	struct __dirent *dirent;

	dirent = malloc(sizeof(struct __dirent));

	return dirent;
}

static int init_dirent(struct __dirent *dirent, struct inode *dir,
		struct dentry *dentry)
{
	dirent->inode = dentry->d_inode;
	strncpy(dirent->name, dentry->d_name, NAME_MAX);
	list_add_tail(&dirent->list, TMPFS_DIRLIST(dir));
	dir->i_size++;

	return 0;
}

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

	struct __dirent *new = alloc_dirent();
	if (new == NULL)
		return -1;
	init_dirent(new, dir, dentry);

	return 0;
}

const struct inode_operations tmpfs_iops;

struct inode *__tmpfs_create(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;
	struct __dirent *dirent;
	static ino_t ino = 300;

	inode = malloc(sizeof(struct inode)); //FIXME: alloc_inode(), or get from cache
	if (inode == NULL)
		return NULL;
	inode->i_ino = ino++;
	inode->i_size = 0;
	inode->i_op = &tmpfs_iops;
	inode->i_mode |= mode;

	dirent = alloc_dirent();
	if (dirent == NULL) {
		free(inode); //FIXME: delete(inode);
		return NULL;
	}
	dentry->d_inode = inode;
	init_dirent(dirent, dir, dentry);

	return inode;
}

int tmpfs_create(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;

	inode = __tmpfs_create(dir, dentry, mode);
	if (inode == NULL)
		return -1;

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
		struct __dirent *dirlist;
		struct list_head *head = (struct list_head *)inode->i_private;
		int i = 2;
		list_for_each_entry(dirlist, head, list) {
			if (i++ == file->f_pos) {
				res = dir_emit(ctx, dirlist->name,
					strlen(dirlist->name), dirlist->inode->i_ino, 0);
				break;
			}
		}
	}
	file->f_pos++;

	return res;
}

const struct dentry_operations tmpfs_dops;

struct dentry *tmpfs_lookup(struct inode *dir, struct dentry *target)
{
	struct list_head *head = (struct list_head *)dir->i_private;
	struct __dirent *dirlist;

	list_for_each_entry(dirlist, head, list) {
		if (!strcmp(target->d_name, dirlist->name)) {
			target->d_inode = dirlist->inode;
			target->d_op = &tmpfs_dops;

			return target;
		}
	}

	return NULL;
}

int tmpfs_delete(struct dentry *dentry)
{
	free(dentry);

	return 0;
}

const struct inode_operations tmpfs_iops = {
	.lookup = tmpfs_lookup,
	.link   = tmpfs_link,
};

const struct file_operations tmpfs_fops = {
	.iterate = tmpfs_iterate,
};

const struct dentry_operations tmpfs_dops = {
	.delete = tmpfs_delete,
};

static struct inode tmpfs_inodes[] = {
	{	/* /      - the root directory */
		.i_ino     = 1,
		.i_op      = &tmpfs_iops,
		.i_fop     = &tmpfs_fops,
		.i_mode    = S_IFDIR,
		.i_size    = 2,
		.i_private = &((struct list_head){}),
	},
	{	/* /dev   - essential device files */
		.i_ino     = 2,
		.i_op      = &tmpfs_iops,
		.i_fop     = &tmpfs_fops,
		.i_mode    = S_IFDIR,
		.i_private = &((struct list_head){}),
	},
	{	/* /proc  - process and kernel information as files */
		.i_ino     = 3,
		.i_op      = &tmpfs_iops,
		.i_fop     = &tmpfs_fops,
		.i_mode    = S_IFDIR,
		.i_private = &((struct list_head){}),
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

struct inode *proc_inode(void)
{
	return &tmpfs_inodes[2];
}

struct dentry *root_dentry(void)
{
	static struct dentry dentry = {
		.d_name   = "/",
		.d_inode  = &tmpfs_inodes[0],
		.d_parent = &dentry,
		.d_op     = &tmpfs_dops,
	};

	return &dentry;
}

struct inode *init_tmpfs_inode(struct inode *inode)
{
	static ino_t ino = 9000;

	inode->i_ino = ino++;
	inode->i_op = &tmpfs_iops;

	return inode;
}

void tmpfs_init(void)
{
	static struct __dirent entries[] = {
		{ .inode = &tmpfs_inodes[1], .name = "dev",  },
		{ .inode = &tmpfs_inodes[2], .name = "proc", },
	};

	struct list_head *rootdir = (struct list_head *)tmpfs_inodes[0].i_private;
	INIT_LIST_HEAD(rootdir);
	list_add_tail(&entries[0].list, rootdir);
	list_add_tail(&entries[1].list, rootdir);

	struct list_head *devdir = (struct list_head *)tmpfs_inodes[1].i_private;
	INIT_LIST_HEAD(devdir);

	struct list_head *procdir = (struct list_head *)tmpfs_inodes[2].i_private;
	INIT_LIST_HEAD(procdir);
}
