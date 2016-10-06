/*
 * include/kernel/device.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_DEVICE_H
#define KERNEL_DEVICE_H

#include "linux/list.h"

#define CDEV_NAME_SIZE  32

typedef unsigned long dev_t;

struct cdevops {
	int (*cdevop_open)();
	int (*cdevop_close)();
	int (*cdevop_read)();
	int (*cdevop_write)();
};

struct cdev {
	char cdev_name[CDEV_NAME_SIZE];
	const struct cdevops *cdev_ops; // should be vnodeops/fileops like Linux
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
