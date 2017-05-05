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

#endif /* !_KERNEL_DEVICE_H */
