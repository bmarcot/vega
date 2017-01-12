/*
 * drivers/char/mem.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

int mem_open(struct inode *inode, struct file *file)
{
	(void)inode, (void)file;

	return 0;
}

ssize_t write_mem(struct file *file, const char *buf, size_t count,
		off_t *offset)
{
	(void)file;

	memcpy((void *)offset, buf, count);

	return count;
}

ssize_t read_mem(struct file *file, char *buf, size_t count, off_t offset)
{
	(void)file;

	memcpy(buf, (void *)offset, count);

	return count;
}

ssize_t write_null(struct file *file, const char *buf, size_t count,
		off_t *offset)
{
	(void)file, (void)buf, (void)offset;

	return count;
}

ssize_t read_null(struct file *file, char *buf, size_t count, off_t offset)
{
	(void)file, (void)buf, (void)count, (void)offset;

	return 0;
}

ssize_t read_zero(struct file *file, char *buf, size_t count, off_t offset)
{
	(void)file, (void)offset;

	memset(buf, 0, count);

	return count;
}

static const struct file_operations mem_fops = {
	.open = mem_open,
	.read = read_mem,
	.write = write_mem,
};

static const struct file_operations null_fops = {
	.open = mem_open,
	.read = read_null,
	.write = write_null,
};

static const struct file_operations zero_fops = {
	.open = mem_open,
	.read = read_zero,
	.write = write_null,
};

extern struct inode tmpfs_inodes[];
extern struct dentry tmpfs_dentries[];
extern const struct file_operations random_fops;
extern const struct inode_operations tmpfs_iops;

static struct inode memdev_inodes[] = {
	{	/* /dev/mem */
		.i_ino  = 101,
		.i_op   = &tmpfs_iops,
		.i_fop  = &mem_fops,
	},
	{	/* /dev/null */
		.i_ino  = 102,
		.i_op   = &tmpfs_iops,
		.i_fop  = &null_fops,
	},
	{	/* /dev/zero */
		.i_ino  = 103,
		.i_op   = &tmpfs_iops,
		.i_fop  = &zero_fops,
	},
	{	/* /dev/random */
		.i_ino  = 104,
		.i_op   = &tmpfs_iops,
		.i_fop  = &random_fops,
	},
};

static struct dentry memdev_dentries[] = {
	{	.d_inode  = &memdev_inodes[0],
		.d_parent = &tmpfs_dentries[1],
		.d_name   = "mem",
	},
	{	.d_inode  = &memdev_inodes[1],
		.d_parent = &tmpfs_dentries[1],
		.d_name   = "null",
	},
	{	.d_inode  = &memdev_inodes[2],
		.d_parent = &tmpfs_dentries[1],
		.d_name   = "zero",
	},
	{	.d_inode  = &memdev_inodes[3],
		.d_parent = &tmpfs_dentries[1],
		.d_name   = "random",
	},
};

void memdev_init(void)
{
	for (int i = 0; i < 4; i++) {
		printk("Creating /dev/%s\n", memdev_dentries[i].d_name);
		tmpfs_mkdir(&memdev_inodes[i], &memdev_dentries[i], 0);
		tmpfs_link(NULL, &tmpfs_inodes[1], &memdev_dentries[i]);
	}
}
