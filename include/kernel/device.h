/*
 * include/kernel/device.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_DEVICE_H
#define _KERNEL_DEVICE_H

#include <kernel/types.h>

#define MINORBITS      10
#define MINORMASK      ((1U << MINORBITS) - 1)

#define MAJOR(dev)     ((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)     ((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma, mi)  (((ma) << MINORBITS) | (mi))

struct file_operations;

int chrdev_register(unsigned int major, const struct file_operations *fops);
int chrdev_add(kdev_t dev, const char *name);

struct device_driver;

struct device {
	struct device_driver *driver;

	void *platform_data; /* Platform specific data, device core doesn't touch it */
	void *driver_data;   /* Driver data, set and get with dev_set/get_drvdata */

	dev_t devt; /* dev_t, creates the sysfs "dev" */
	void (*release)(struct device *dev);
	int offline:1;

	struct list_head list; //XXX: Legacy, find a better device hierarchy
};

struct device_driver {
	int (*probe) (struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev);
	int (*resume) (struct device *dev);
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

//XXX: Helper functions before we get a correct device model
int device_add(struct device *dev);
struct device *device_get(dev_t devt);
struct device *device_alloc(void);

#endif /* !_KERNEL_DEVICE_H */
