/*
 * include/kernel/device.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_DEVICE_H
#define KERNEL_DEVICE_H

#include <kernel/fs/vnode.h>
#include "linux/list.h"

#define CDEV_NAME_SIZE  32

typedef unsigned long dev_t;

struct cdev {
	char cdev_name[CDEV_NAME_SIZE];
	const struct vnodeops *cdev_vops;
	dev_t cdev_major;
	//struct list_head cdev_list;
};

struct device {
	struct cdev *char_dev; /* XXX: will die - should use the dev id to
				  retrieve the dev struct, but easier this way */
	void *drvdata;
	dev_t devt;
	struct list_head dev_list;
};

int mkdev(struct device *dev, const char *devname);

#endif /* !KERNEL_DEVICE_H */
