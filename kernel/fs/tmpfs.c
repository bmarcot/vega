/*
 * kernel/fs/tmpfs.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/fdtable.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/types.h>

const struct inode_operations tmpfs_iops;
const struct file_operations tmpfs_fops;
const struct dentry_operations tmpfs_dops;

int tmpfs_link(struct dentry *old_dentry, struct inode *dir,
	struct dentry *dentry)
{
	(void)old_dentry, (void)dir, (void)dentry;

	return -1;
}

struct inode *tmpfs_iget(struct super_block *sb, int mode)
{
	struct inode *inode;

	inode = new_inode(sb);
	if (!inode)
		return NULL;

	inode->i_op = &tmpfs_iops;
	inode->i_fop = &tmpfs_fops;
	inode->i_mode = mode;

	return inode;
}

struct inode *__tmpfs_create(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;

	inode = tmpfs_iget(dir->i_sb, mode);
	if (!inode)
		return NULL;

	return inode;
}

struct inode *__tmpfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;

	inode = tmpfs_iget(dir->i_sb, mode);
	if (!inode)
		return NULL;
	inode->i_fop = &tmpfs_fops;

	return inode;
}

struct inode *__tmpfs_mknod(struct inode *dir, struct dentry *dentry,
			umode_t mode, dev_t dev)
{
	struct inode *inode;

	inode = tmpfs_iget(dir->i_sb, mode);
	if (!inode)
		return NULL;
	init_special_inode(inode, mode, dev);

	return inode;
}

int tmpfs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
		int /* bool */ exclusive)
{
	struct inode *inode;

	inode = __tmpfs_create(dir, dentry, mode | S_IFREG);
	if (!inode)
		return -1;

	d_instantiate(dentry, inode);
	dir->i_size++;

	return 0;
}

int tmpfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	struct inode *inode;

	inode = __tmpfs_mkdir(dir, dentry, mode | S_IFDIR);
	if (!inode)
		return -1;

	d_instantiate(dentry, inode);
	dir->i_size++;

	return 0;
}

int tmpfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode,
		dev_t dev)
{
	struct inode *inode;

	inode = __tmpfs_mknod(dir, dentry, mode, dev);
	if (!inode)
		return -1;

	d_instantiate(dentry, inode);
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
		int i = 2;
		struct dentry *dentry;
		list_for_each_entry(dentry, &file->f_dentry->d_subdirs, d_child) {
			if (i++ == file->f_pos) {
				res = dir_emit(ctx, dentry->d_name,
					strlen(dentry->d_name), dentry->d_inode->i_ino, 0);
				break;
			}
		}
	}
	file->f_pos++;

	return res;
}

struct dentry *tmpfs_lookup(struct inode *dir, struct dentry *target)
{
	/* Dentries mapping to tmpfs inodes always live in memory */
	return NULL;
}

int tmpfs_delete(struct dentry *dentry)
{
	return 0;
}

const struct super_operations tmpfs_sops = {0};

const struct inode_operations tmpfs_iops = {
	.lookup = tmpfs_lookup,
	.create = tmpfs_create,
	.link   = tmpfs_link,
	.mkdir  = tmpfs_mkdir,
	.mknod  = tmpfs_mknod,
};

const struct file_operations tmpfs_fops = {
	.iterate = tmpfs_iterate,
};

const struct dentry_operations tmpfs_dops = {
	.delete = tmpfs_delete,
};

static LIST_HEAD(root_dirlist);

static struct super_block super_root = {
	.s_op = &tmpfs_sops,
};

static struct dentry de_root;

static struct inode in_root = {
	.i_mode    = S_IFDIR,
	.i_ino     = 1,
	.i_op      = &tmpfs_iops,
	.i_fop     = &tmpfs_fops,
	.i_sb      = &super_root,
	.i_dentry  = &de_root,
};

static struct dentry de_root = {
	.d_count  = -1, /* sticky file */
	.d_inode  = &in_root,
	.d_op     = &tmpfs_dops,
	.d_parent = &de_root,
	.d_name   = "/",
};

struct inode *root_inode(void)
{
	return &in_root;
}

struct dentry *root_dentry(void)
{
	return &de_root;
}

void tmpfs_init(void)
{
	INIT_LIST_HEAD(&super_root.s_inodes);
	INIT_LIST_HEAD(&de_root.d_subdirs);
}

/*
 * Filesystem helper functions
 */

struct inode *creat_file(struct inode *dir, const char *filename)
{
	struct dentry *dentry;

	dentry = d_alloc(dir->i_dentry, filename);
	if (!dentry)
		return NULL;

	if (dir->i_op->create(dir, dentry, S_IFREG, 0)) {
		d_put(dentry);
		return NULL;
	}

	return dentry->d_inode;
}

struct inode *make_dir(struct inode *dir, const char *filename)
{
	struct dentry *dentry;

	dentry = d_alloc(dir->i_dentry, filename);
	if (!dentry)
		return NULL;

	if (dir->i_op->mkdir(dir, dentry, 0)) {
		d_put(dentry);
		return NULL;
	}

	return dentry->d_inode;
}

struct inode *make_nod(struct inode *dir, const char *filename, umode_t mode,
		dev_t dev)
{
	struct dentry *dentry;

	dentry = d_alloc(dir->i_dentry, filename);
	if (!dentry)
		return NULL;

	if (dir->i_op->mknod(dir, dentry, mode, dev)) {
		d_put(dentry);
		return NULL;
	}

	return dentry->d_inode;
}
