/*
 * kernel/device.c
 *
 * Copyright (c) 2017-2019 Benoit Marcot
 */

#include <kernel/cdev.h>
#include <kernel/device.h>
#include <kernel/errno-base.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/stdio.h>
#include <kernel/string.h>
#include <kernel/types.h>

#include <uapi/kernel/stat.h>

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

const struct file_operations def_chr_fops = {
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

int chrdev_add(kdev_t dev, const char *name)
{
	struct inode *inode;

	inode = make_nod(dev_inode(), name, 0666 | S_IFCHR, dev);
	if (!inode) {
		pr_err("Cannot add device %s (%d, %d)", name, MAJOR(dev),
			MINOR(dev));
		return -1;
	}
	inode->i_dentry->d_count = -1; /* sticky file */

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
	struct device *dev = kmalloc(sizeof(struct device));

	return dev;
}

static struct inode *dev_i;

struct inode *dev_inode(void)
{
	return dev_i;
}

int devfs_init(void)
{
	/* create /dev */
	dev_i = make_dir(root_inode(), "dev");
	if (!dev_i)
		return -1;
	dev_i->i_dentry->d_count = -1; /* sticky file */

	return 0;
}

/* Character device interface */

static int cdev_open(struct inode *inode, struct file *file)
{
	struct cdev *cdev = inode->i_cdev;

	if (!cdev)
		return -ENXIO;
	/* if (!cdev) */
	/* 	kobj_lookup(cdev_map, inode->i_rdev, &idx); */
	file->f_op = cdev->ops;
	if (file->f_op->open)
		return file->f_op->open(inode, file);

	return 0;
}

static const struct file_operations cdev_fops = {
	.open = cdev_open,
};

#define CHRDEV_MAJOR_HASH_SIZE 4

static struct char_device_struct {
	struct char_device_struct	*next;
	unsigned int			major;
	unsigned int			baseminor;
	int				minorct;
	char				name[16];
	const struct file_operations	*fops;
} chrdevs[CHRDEV_MAJOR_HASH_SIZE];

int register_chrdev(unsigned int major, unsigned int baseminor,
		unsigned int count, const char *name,
		const struct file_operations *fops)
{
	struct char_device_struct *cd = &chrdevs[major];

	BUG_ON(major >= CHRDEV_MAJOR_HASH_SIZE);

	cd->fops = fops;
	cd->baseminor = baseminor;
	cd->minorct = 0;
	strcpy(cd->name, name);

	return 0;
}

int cdev_add(struct cdev *p, dev_t dev)
{
	struct char_device_struct *cd = &chrdevs[MAJOR(dev)];
	char name[16];
	struct inode *inode;

	sprintf(name, "%s%d", cd->name, MINOR(dev));

	//XXX: Call make_nod() but overload the inode i_fop with the cdev operations
	inode = make_nod(dev_inode(), name, 0666 | S_IFCHR, dev);
	if (!inode) {
		pr_err("Cannot add device %s (%d, %d)", name, MAJOR(dev),
			MINOR(dev));
		return -1;
	}
	inode->i_dentry->d_count = -1; /* sticky file */
	inode->i_cdev = p;
	inode->i_fop = &cdev_fops;

	p->dev = dev;
	p->ops = cd->fops;

	return 0;
}
