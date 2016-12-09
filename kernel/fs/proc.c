/*
 * kernel/fs/proc.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

#include "linux/list.h"

#define INO_PROC_BASE 0x100

static struct inode proc_inodes[] = {
	{	/* /proc/version */
		.i_mode = S_IFREG,
		.i_ino  = INO_PROC_BASE,
	},
};

extern struct dentry rootdir_dentries[];

static struct dentry proc_dentries[] = {
	{	.d_inode  = &proc_inodes[0],
		.d_parent = &rootdir_dentries[2],
		.d_name   = "version",
	},
	{	/* placeholder */
		.d_inode  = &((struct inode){.i_ino = 45}),
		.d_parent = &rootdir_dentries[2],
		.d_name   = "version",
	},
};

int proc_iterate(struct file *file, struct dir_context *ctx)
{
	int res = -1;
	int pos = file->f_pos;

	if (pos >= 2 + (int)ARRAY_SIZE(proc_dentries))
		return -1;
	if (!pos) {
		res = dir_emit_dot(file, ctx);
	} else if (file->f_pos == 1) {
		res = dir_emit_dotdot(file, ctx);
	} else {
		char *name = proc_dentries[pos - 1].d_name;
		ino_t ino = proc_dentries[pos - 1].d_inode->i_ino;
		res = dir_emit(ctx, name, strlen(name), ino, 0);
	}
	file->f_pos++;

	return res;
}

struct dentry *proc_lookup(struct inode *dir, struct dentry *target)
{
	(void)dir;

	for (int i = 0; i < (int)ARRAY_SIZE(proc_dentries); i++) {
		if (!strcmp(target->d_name, proc_dentries[i].d_name))
			return &proc_dentries[i];
	}

	return NULL;
}

const struct inode_operations proc_iops = {
	.lookup = proc_lookup,
};

const struct file_operations proc_fops = {
	.iterate = proc_iterate,
};
