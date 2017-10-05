/*
 * kernel/fs/tmpfs.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/stat.h>
#include <kernel/types.h>

const struct inode_operations tmpfs_iops;
const struct file_operations tmpfs_fops;
const struct dentry_operations tmpfs_dops;

struct tmpfs_dirent {
	struct inode		*inode;
	char			name[NAME_MAX];
	struct list_head	list;
};

#define TMPFS_DIRLIST(dir) \
	((struct list_head *)(dir)->i_private)

static int init_dirent(struct inode *inode, struct inode *dir,
		struct dentry *dentry)
{
	struct tmpfs_dirent *dirent = (struct tmpfs_dirent *)(inode + 1);

	dirent->inode = inode;
	strncpy(dirent->name, dentry->d_name, NAME_MAX);
	list_add_tail(&dirent->list, TMPFS_DIRLIST(dir));
	dir->i_size++;

	return 0;
}

int tmpfs_link(struct dentry *old_dentry, struct inode *dir,
	struct dentry *dentry)
{
	(void)old_dentry, (void)dir, (void)dentry;

	return -1;
}

struct inode *tmpfs_iget(struct super_block *sb, unsigned long ino, int mode)
{
	struct inode *inode;

	if (S_ISDIR(mode))
		inode = kmalloc(sizeof(struct inode) + sizeof(struct tmpfs_dirent)
				+ sizeof(struct list_head));
	else
		inode = kmalloc(sizeof(struct inode) + sizeof(struct tmpfs_dirent));
	if (!inode)
		return NULL;
	inode->i_ino = ino;
	inode->i_size = 0;
	inode->i_op = &tmpfs_iops;
	inode->i_fop = NULL;
	inode->i_mode = mode;
	inode->i_sb = sb;

	return inode;
}

static ino_t ino = 10000;

struct inode *__tmpfs_create(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;

	inode = tmpfs_iget(NULL, ino++, mode);
	if (!inode)
		return NULL;
	init_dirent(inode, dir, dentry);

	return inode;
}

struct inode *__tmpfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;

	inode = tmpfs_iget(NULL, ino++, mode);
	if (!inode)
		return NULL;
	init_dirent(inode, dir, dentry);

	struct list_head *dirlist =
		(struct list_head *)((u32)(inode + 1) + (u32)(sizeof(struct tmpfs_dirent)));
	INIT_LIST_HEAD(dirlist);
	inode->i_private = dirlist;
	inode->i_fop = &tmpfs_fops;

	return inode;
}

struct inode *__tmpfs_mknod(struct inode *dir, struct dentry *dentry,
			umode_t mode, dev_t dev)
{
	struct inode *inode;

	inode = tmpfs_iget(NULL, ino++, mode);
	if (!inode)
		return NULL;
	init_dirent(inode, dir, dentry);
	init_special_inode(inode, mode, dev);

	return inode;
}

int tmpfs_create(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode = __tmpfs_create(dir, dentry, mode | S_IFREG);

	if (!inode)
		return -1;
	return 0;
}

int tmpfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode = __tmpfs_mkdir(dir, dentry, mode | S_IFDIR);

	if (!inode)
		return -1;
	return 0;
}

int tmpfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode,
		dev_t dev)
{
	struct inode *inode = __tmpfs_mknod(dir, dentry, mode, dev);

	if (!inode)
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
		struct tmpfs_dirent *dirlist;
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

struct dentry *tmpfs_lookup(struct inode *dir, struct dentry *target)
{
	struct list_head *head = (struct list_head *)dir->i_private;
	struct tmpfs_dirent *dirlist;

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
	kfree(dentry);

	return 0;
}

const struct inode_operations tmpfs_iops = {
	.lookup = tmpfs_lookup,
	.link   = tmpfs_link,
	.mkdir  = tmpfs_mkdir,
};

const struct file_operations tmpfs_fops = {
	.iterate = tmpfs_iterate,
};

const struct dentry_operations tmpfs_dops = {
	.delete = tmpfs_delete,
};

static LIST_HEAD(root_dirlist);

static struct inode in_root = {
	.i_ino     = 1,
	.i_op      = &tmpfs_iops,
	.i_fop     = &tmpfs_fops,
	.i_mode    = S_IFDIR,
	.i_private = &root_dirlist,
};

struct inode *root_inode(void)
{
	return &in_root;
}

struct dentry *root_dentry(void)
{
	static struct dentry dentry = {
		.d_name   = "/",
		.d_inode  = &in_root,
		.d_parent = &dentry,
		.d_op     = &tmpfs_dops,
	};

	return &dentry;
}

struct inode *init_tmpfs_inode(struct inode *inode)
{
	static ino_t ino = 11000;

	inode->i_ino = ino++;
	inode->i_op = &tmpfs_iops;

	return inode;
}

struct inode *creat_file(struct inode *dir, const char *filename,
			const struct file_operations *fops)
{
	struct inode *inode;
	struct dentry dentry;

	strncpy(dentry.d_name, filename, NAME_MAX);
	inode = __tmpfs_create(dir, &dentry, S_IFREG);
	if (!inode)
		return NULL;
	inode->i_fop = fops;

	return inode;
}

struct inode *make_dir(struct inode *dir, const char *filename)
{
	struct inode *inode;
	struct dentry dentry;

	strncpy(dentry.d_name, filename, NAME_MAX);
	inode = __tmpfs_mkdir(dir, &dentry, S_IFDIR);
	if (!inode)
		return NULL;

	return inode;
}

struct inode *make_nod(struct inode *dir, const char *filename, umode_t mode,
		dev_t dev)
{
	struct inode *inode;
	struct dentry dentry;

	strncpy(dentry.d_name, filename, NAME_MAX);
	inode = __tmpfs_mknod(dir, &dentry, mode, dev);
	if (!inode)
		return NULL;

	return inode;
}
