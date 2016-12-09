/*
 * kernel/fs/rootdir.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

//XXX: This is actually a ramfs/tmpfs filesystem, revisit later

static const struct inode_operations rootdir_iops;
static const struct file_operations rootdir_fops;

extern const struct inode_operations dev_iops;
extern const struct file_operations dev_fops;

extern const struct inode_operations proc_iops;
extern const struct file_operations proc_fops;

struct inode rootdir_inodes[] = {
	{	/* /      - the root directory */
		.i_mode = S_IFDIR,
		.i_ino  = 1,
		.i_op   = &rootdir_iops,
		.i_fop  = &rootdir_fops,
	},
	{	/* /dev   - essential device files */
		.i_mode = S_IFDIR,
		.i_ino  = 2,
		.i_op   = &dev_iops,
		.i_fop  = &dev_fops,
	},
	{	/* /proc  - process and kernel information as files */
		.i_mode = S_IFDIR,
		.i_ino  = 3,
		.i_op   = &proc_iops,
		.i_fop  = &proc_fops,
	},
};

struct dentry rootdir_dentries[] = {
	{	.d_inode  = &rootdir_inodes[0],
		.d_parent = &rootdir_dentries[0],
	},
	{	.d_inode  = &rootdir_inodes[1],
		.d_parent = &rootdir_dentries[0],
		.d_name   = "dev",
	},
	{	.d_inode  = &rootdir_inodes[2],
		.d_parent = &rootdir_dentries[0],
		.d_name   = "proc",
	},
};

// iterate_dentry_table(*table, len)
int rootdir_iterate(struct file *file, struct dir_context *ctx)
{
	int res = -1;
	int pos = file->f_pos;

	if (pos >= 2 + (int)ARRAY_SIZE(rootdir_dentries))
		return -1;
	if (!pos) {
		res = dir_emit_dot(file, ctx);
	} else if (file->f_pos == 1) {
		res = dir_emit_dotdot(file, ctx);
	} else {
		char *name = rootdir_dentries[pos - 1].d_name;
		ino_t ino = rootdir_dentries[pos - 1].d_inode->i_ino;
		res = dir_emit(ctx, name, strlen(name), ino, 0);
	}
	file->f_pos++;

	return res;
}

struct dentry *rootdir_lookup(struct inode *dir, struct dentry *target)
{
	(void)dir;

	for (int i = 0; i < (int)ARRAY_SIZE(rootdir_dentries); i++) {
		if (!strcmp(target->d_name, rootdir_dentries[i].d_name))
			return &rootdir_dentries[i];
	}

	return NULL;
}

static const struct inode_operations rootdir_iops = {
	.lookup = rootdir_lookup,
};

static const struct file_operations rootdir_fops = {
	.iterate = rootdir_iterate,
};
