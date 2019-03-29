/*
 * include/kernel/cdev.h
 *
 * Copyright (c) 2019 Benoit Marcot
 */

#ifndef _KERNEL_CDEV_H
#define _KERNEL_CDEV_H

#include <kernel/types.h>

struct file_operations;

struct cdev {
	/* struct kobject kobj; */
	const struct file_operations *ops;
	struct list_head	list;
	dev_t			dev;
	unsigned int		count;
};

int cdev_add(struct cdev *p, dev_t dev);

#endif /* !_KERNEL_CDEV_H */
