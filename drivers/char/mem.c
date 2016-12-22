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

struct memdev {
	const char *name;
	const struct file_operations *fops;
};

extern struct file_operations random_fops;

struct memdev devlist[] = {
	{ "mem",    &mem_fops    },
	{ "null",   &null_fops   },
	{ "zero",   &zero_fops   },
	{ "random", &random_fops },
};

struct inode *create_dev_inode(const char *name,
			const struct file_operations *fops);

void devfs_mem_init(void)
{
	for (int i = 0; i < 4; i++) {
		printk("Creating /dev/%s\n", devlist[i].name);
		create_dev_inode(devlist[i].name, devlist[i].fops);
	}
}
