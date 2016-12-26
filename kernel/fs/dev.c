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

LIST_HEAD(dev_pairs);

struct dev_pair {
	struct inode inode;
	struct dentry dentry;
	struct list_head list;
};

extern struct dentry rootdir_dentries[];

struct inode *create_dev_inode(const char *name, const struct file_operations *fops)
{
	struct dev_pair *dev = malloc(sizeof(struct dev_pair));
	static ino_t dev_ino = 0x1000;

	dev->inode.i_ino = dev_ino++;
	dev->inode.i_mode = S_IFREG;
	dev->inode.i_fop = fops;
	strcpy(dev->dentry.d_name, name);
	dev->dentry.d_parent = &rootdir_dentries[1];
	dev->dentry.d_inode = &dev->inode;
	list_add(&dev->list, &dev_pairs);
	rootdir_dentries[1].d_inode->i_size++;

	return &dev->inode;
}

int dev_iterate(struct file *file, struct dir_context *ctx)
{
	int res = -1;
	int pos = file->f_pos;

	if (pos == rootdir_dentries[1].d_inode->i_size + 2) {
		return -1;
	} else if (!pos) {
		res = dir_emit_dot(file, ctx);
	} else if (file->f_pos == 1) {
		res = dir_emit_dotdot(file, ctx);
	} else {
		struct dev_pair *pair;
		int i = 0;
		list_for_each_entry(pair, &dev_pairs, list) {
			if (i++ == pos) {
				res = dir_emit(ctx, pair->dentry.d_name,
					strlen(pair->dentry.d_name), pair->inode.i_ino, 0);
				break;
			}
		}
	}
	file->f_pos++;

	return res;
}

struct dentry *dev_lookup(struct inode *dir, struct dentry *target)
{
	(void)dir;

	struct dev_pair *pair;
	list_for_each_entry(pair, &dev_pairs, list) {
		if (!strcmp(target->d_name, pair->dentry.d_name)) {
			return &pair->dentry;
		}
	}

	return NULL;
}

const struct inode_operations dev_iops = {
	.lookup = dev_lookup,
};

const struct file_operations dev_fops = {
	.iterate = dev_iterate,
};
