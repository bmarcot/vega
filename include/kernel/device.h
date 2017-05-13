/*
 * include/kernel/device.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_DEVICE_H
#define _KERNEL_DEVICE_H

#include <kernel/fs.h>
#include <kernel/types.h>

#define MINORBITS      10
#define MINORMASK      ((1U << MINORBITS) - 1)

#define MAJOR(dev)     ((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)     ((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma, mi)  (((ma) << MINORBITS) | (mi))

int chrdev_register(unsigned int major, const struct file_operations *fops);
int chrdev_add(kdev_t dev, const char *name);

struct device {
	/* struct device_driver *driver; */
	void                 *driver_data;
	dev_t                devt;
	/* __u32                id; */
	struct list_head     list;
};

static inline void *dev_get_drvdata(const struct device *dev)
{
	return dev->driver_data;
}

static inline void dev_set_drvdata(struct device *dev, void *data)
{
	dev->driver_data = data;
}

static inline void dev_set_devt(struct device *dev, dev_t devt)
{
	dev->devt = devt;
}

int device_add(struct device *dev);
struct device *device_get(dev_t devt);
struct device *device_alloc(void);

#endif /* !_KERNEL_DEVICE_H */
