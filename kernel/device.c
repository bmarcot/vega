/*
 * kernel/device.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <kernel/device.h>
#include <kernel/errno-base.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>

static struct char_dev {
	const struct file_operations *fops;
	unsigned int                 count;
} char_devs[8];

static inline struct char_dev *chrdev_lookup(unsigned int major)
{
	return &char_devs[major];
}

static int chrdev_open(struct inode *inode, struct file *file)
{
	struct char_dev *chrdev;

	chrdev = chrdev_lookup(MAJOR(inode->i_rdev));
	if (chrdev == NULL)
		return -ENXIO;
	file->f_op = chrdev->fops;
	if (file->f_op->open)
		return file->f_op->open(inode, file);

	return 0;
}

static const struct file_operations def_chr_fops = {
	.open = chrdev_open,
};

int chrdev_register(unsigned int major, const struct file_operations *fops)
{
	struct char_dev *chrdev;

	chrdev = chrdev_lookup(major);
	if (chrdev == NULL) {
		pr_err("Cannot register device with major=%d", major);
		return -ENXIO;
	}
	chrdev->fops = fops;
	chrdev->count = 0;

	return 0;
}

void init_special_inode(struct inode *inode, umode_t mode, kdev_t rdev)
{
	inode->i_mode = mode;
	if (S_ISCHR(mode)) {
		inode->i_fop = &def_chr_fops;
		inode->i_rdev = rdev;
	}
}

int chrdev_add(kdev_t dev, const char *name)
{
	struct dentry dentry;
	struct inode *inode;

	strcpy(dentry.d_name, name);
	inode = __tmpfs_create(dev_inode(), &dentry, 0);
	if (inode == NULL) {
		pr_err("Cannot add device %s (%d, %d)", name, MAJOR(dev),
			MINOR(dev));
		return -1;
	}
	init_special_inode(inode, 0666 | S_IFCHR, dev);

	return 0;
}

static LIST_HEAD(device_head);

int device_add(struct device *dev)
{
	list_add(&dev->list, &device_head);

	return 0;
}

struct device *device_get(dev_t devt)
{
	struct device *dev;

	list_for_each_entry(dev, &device_head, list) {
		if (dev->devt == devt)
			return dev;
	}

	return NULL;
}

struct device *device_alloc(void)
{
	//FIXME: Get memory from dev-cache
	struct device *dev = malloc(sizeof(struct device));

	return dev;
}
