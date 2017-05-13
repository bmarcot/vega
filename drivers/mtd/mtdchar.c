/*
 * drivers/mtd/mtdchar.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/device.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>

#include <drivers/mtd/mtd.h>
#include <drivers/mtd/mtdchar.h>

static ssize_t mtdchar_read(struct file *file, char *buf, size_t count,
			off_t offset)
{
	struct mtd_info *mtd;
	size_t retlen;

	mtd = get_mtd_device(file->f_dentry->d_inode->i_rdev);
	if (mtd == NULL)
		return -ENXIO;
	if (mtd_read(mtd, offset, count, &retlen, (unsigned char *)buf) < 0)
		return -1;

	return retlen;
}

static ssize_t mtdchar_write(struct file *file, const char *buf, size_t count,
			off_t *offset)
{
	struct mtd_info *mtd;
	size_t retlen;

	mtd = get_mtd_device(file->f_dentry->d_inode->i_rdev);
	if (mtd == NULL)
		return -ENXIO;
	if (mtd_write(mtd, *offset, count, &retlen, (const unsigned char *)buf) < 0)
		return -1;

	return retlen;
}

static const struct file_operations mtdchar_fops = {
	.read  = mtdchar_read,
	.write = mtdchar_write,
};

int mtdchar_init(void)
{
	return chrdev_register(MTDCHAR_MAJOR, &mtdchar_fops);
}
