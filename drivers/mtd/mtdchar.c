/*
 * drivers/mtd/mtdchar.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/device.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/types.h>

#include <drivers/mtd/mtd.h>
#include <drivers/mtd/mtdchar.h>

static int mtdchar_open(struct inode *inode, struct file *file)
{
	struct mtd_info *mtd;

	mtd = get_mtd_device(inode->i_rdev);
	if (mtd == NULL)
		return -ENXIO;
	file->f_private = mtd;

	return 0;
}

static ssize_t mtdchar_read(struct file *file, char *buf, size_t count,
			off_t offset)
{
	struct mtd_info *mtd = file->f_private;
	size_t retlen;
	int err;

	err = mtd_read(mtd, offset, count, &retlen, (unsigned char *)buf);
	if (err < 0)
		return -1;

	return retlen;
}

static ssize_t mtdchar_write(struct file *file, const char *buf, size_t count,
			off_t *offset)
{
	struct mtd_info *mtd = file->f_private;
	size_t retlen;
	int err;

	err = mtd_write(mtd, *offset, count, &retlen,
			(const unsigned char *)buf);
	if (err < 0)
		return -1;

	return retlen;
}

static const struct file_operations mtdchar_fops = {
	.open  = mtdchar_open,
	.read  = mtdchar_read,
	.write = mtdchar_write,
};

int mtdchar_init(void)
{
	return chrdev_register(MTDCHAR_MAJOR, &mtdchar_fops);
}
