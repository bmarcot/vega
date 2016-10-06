/*
 * kernel/device.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/device.h>
#include <kernel/fs/vnode.h>

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
	vn->v_ops = &cdev_vops;
	vn->v_data = dev;
	INIT_LIST_HEAD(&vn->v_head);
	vn_insert(vn, &vn_dev);

	return 0;
}

// mkdev_const_allocated(const struct device *dev, const char *name)
