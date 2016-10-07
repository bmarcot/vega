/*
 * kernel/device.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/device.h>
#include <kernel/fs/vnode.h>

#include "linux/list.h"

// major number = device type (nrf52 temp, qemu uart...)
// minor number = device instance

/* register_chardev(cdevops);                -- get device number (major) */
/* device_create(device_number, device name) -- create a vnode (minor)    */

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

/* /\* struct device *device_create(struct class *class, struct device *parent, *\/ */
/* /\* 			dev_t devt, void *drvdata, const char *fmt, ...) *\/ */
/* struct device *device_create(dev_t devt, void *drvdata, */
/* 			const char *name_in_dev, struct cdev *char_dev) */
/* { */
/* 	struct device *dev; */

/* 	dev = malloc(sizeof(struct device)); */

/* 	// find char dev by major number */
/* 	dev->devt = devt; */
/* 	dev->drvdata = drvdata; */
/* 	//list_add(..., &device_list); */
/* 	// create vnode here? */

/* 	return dev; */
/* } */

extern struct vnode vn_dev;
extern struct vnodeops cdev_vops;

/* make a char device */
int mkdev(struct device *dev, const char *devname)
{
	struct vnode *vn = vn_alloc();

	if (vn == NULL)
		return -1;
	vn->v_path = (char *)devname;
	vn->v_type = VCHR;
	vn->v_ops = dev->char_dev->cdev_vops;
	vn->v_data = dev; /* union { v_data,  v_dev}; */
	INIT_LIST_HEAD(&vn->v_head);
	vn_insert(vn, &vn_dev);

	return 0;
}

// mkdev_const_allocated(const struct device *dev, const char *name)
