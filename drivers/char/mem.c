/*
 * drivers/char/mem.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <string.h>

#include <kernel/device.h>
#include <kernel/errno-base.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>

#define MEM_MAJOR  1

int open_mem(struct inode *inode, struct file *file)
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
	.open = open_mem,
	.read = read_mem,
	.write = write_mem,
};

static const struct file_operations null_fops = {
	.read = read_null,
	.write = write_null,
};

static const struct file_operations zero_fops = {
	.read = read_zero,
	.write = write_null,
};

extern const struct file_operations random_fops;

/* "mem" devices:
 *
 *     PATH      MAJOR  MINOR
 *     /dev/mem      1,     1
 *     /dev/null     1,     2
 *     /dev/zero     1,     3
 *     /dev/random   1,     4  (implements a non-blocking PRNG)
 */
static const struct memdev {
	const char                   *name;
	umode_t                      mode;
	const struct file_operations *fops;
	fmode_t                      fmode;
} devlist[] = {
	[1] = { "mem",    0,    &mem_fops,    0 },
	[2] = { "null",   0666, &null_fops,   0 },
	[3] = { "zero",   0666, &zero_fops,   0 },
	[4] = { "random", 0666, &random_fops, 0 },
};

static int memory_open(struct inode *inode, struct file *file)
{
	int minor;
	const struct memdev *dev;

	minor = MINOR(inode->i_rdev);
	if (minor >= ARRAY_SIZE(devlist))
		return -ENXIO;

	dev = &devlist[minor];
	if (!dev->fops)
		return -ENXIO;

	file->f_op = dev->fops;
	file->f_mode |= dev->fmode;

	if (dev->fops->open)
		return dev->fops->open(inode, file);

	return 0;
}

static const struct file_operations memory_fops = {
        .open = memory_open,
};

void memdev_init(void)
{
	chrdev_register(MEM_MAJOR, &memory_fops);
	for (int i = 1; i < ARRAY_SIZE(devlist); i++)
		chrdev_add(MKDEV(MEM_MAJOR, i), devlist[i].name);
}
