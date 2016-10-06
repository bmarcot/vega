/*
 * kernel/fs/char_dev.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>
#include "linux/list.h"

#include <kernel/device.h>
#include <kernel/fs/vnode.h>

int cdev_open()
{
	return 0;
}
int cdev_close()
{
	return 0;
}

int cdev_read()
{
	return 0;
}

int cdev_write(struct vnode *vp, void *buf, size_t count, off_t off, size_t *n)
{
	struct device *dev;

	dev = vp->v_data;
	if (dev == NULL)
		return -1;

	return dev->char_dev->cdev_ops->cdevop_write(dev, buf, count, off, n);
}

// major number = device type (nrf52 temp, qemu uart...)
// minor number = device instance

/* register_chardev(cdevops);                -- get device number (major) */
/* device_create(device_number, device name) -- create a vnode (minor)    */

const struct vnodeops cdev_vops = {
	.vop_open = cdev_open,
	.vop_close = cdev_close,
	.vop_read = cdev_read,
	.vop_write = cdev_write,
};

/* int register_chrdev(unsigned int major, const char *name, */
/* 		const struct cdevops *ops) */
/* { */
/* 	struct cdev *cdev = malloc(sizeof(struct cdev)); */

/* 	cdev->cdev_major = major; */
/* 	cdev->cdev_ops = ops; */
/* 	strncpy(cdev->cdev_name, name, CDEV_NAME_SIZE); */
/* 	//list_add */

/* 	return 0; */
/* } */
