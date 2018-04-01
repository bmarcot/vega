/*
 * kernel/fs/initfs.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/fs.h>
#include <kernel/fs/initfs.h>
#include <kernel/fs/tmpfs.h>
#include <kernel/kernel.h>
#include <kernel/string.h>

const struct inode_operations initfs_iops;
const struct file_operations initfs_fops;
const struct dentry_operations initfs_dops;

ssize_t initfs_read(struct file *file, char *buf, size_t count, off_t offset)
{
	struct inode *in = file->f_dentry->d_inode;

	if (file->f_pos + (off_t)count > in->i_size)
		count = in->i_size - offset;
	memcpy(buf, (void *)((off_t)in->i_private + offset), count);

	return count;
}

static struct inode *alloc_inode(struct initfs_inode *ii, struct inode *dir)
{
	struct inode *inode;

	/* initfs supports regular files only */
	inode = creat_file(dir, ii->file_name);
	inode->i_fop = &initfs_fops;
	inode->i_size = ii->file_size;
	inode->i_private = (char *)ii + align_next(2 + strlen(ii->file_name) + 1, 4);

	return inode;
}

static struct initfs_inode *get_next_inode(struct initfs_inode *ii)
{
	off_t off = align_next(2 + strlen(ii->file_name) + 1, 4)
		+ align_next(ii->file_size, 4);
	return (struct initfs_inode *)((char *)ii + off);
}

struct dentry *initfs_lookup(struct inode *dir, struct dentry *target)
{
	struct initfs_inode *ii = dir->i_private;

	//FIXME: infinite loop, really?
	for (;;) {
		if (!strcmp(ii->file_name, target->d_name)) {
			struct inode *inode = alloc_inode(ii, dir);
			if (inode == NULL) {
				pr_err("cannot allocate inode");
				return NULL;
			}
			target->d_inode = inode;
			target->d_op = &initfs_dops;

			return target;
		}
		ii = get_next_inode(ii);
	}

	return NULL;
}

// mount(NULL, "/init", "initfs", 0, __init_start);
int initfs_mount(const char *source, const char *target,
		const char *filesystemtype,
		unsigned long mountflags, const void *data)
{
	struct inode *in;

	in = make_dir(root_inode(), "init" /* basename(target) */);
	if (!in)
		return -1;
	in->i_op = &initfs_iops;
	in->i_private = (void *)data;

	return 0;
}

const struct inode_operations initfs_iops = {
	.lookup = initfs_lookup,
	.create = tmpfs_create,
};

const struct file_operations initfs_fops = {
	.read = initfs_read,
	/* .mmap = initfs_mmap, */
};

const struct dentry_operations initfs_dops = {
	/* .delete = initfs_delete, */
};
