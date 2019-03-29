/*
 * include/uapi/kernel/clock.h
 *
 * Copyright (c) 2019 Benoit Marcot
 */

#ifndef _UAPI_KERNEL_CLOCK_H
#define _UAPI_KERNEL_CLOCK_H

#include <uapi/kernel/ioctl.h>
#include <uapi/kernel/time.h>

#define CLOCK_START	_IO('c', 0x01)
#define CLOCK_STOP	_IO('c', 0x02)
#define CLOCK_PIE_ON	_IO('c', 0x03)
#define CLOCK_PIE_OFF	_IO('c', 0x04)
#define CLOCK_SET_TIME	_IOW('c', 0x10, struct timespec)
#define CLOCK_RD_TIME	_IOR('c', 0x11, struct timespec)

#endif /* _UAPI_KERNEL_CLOCK_H */
