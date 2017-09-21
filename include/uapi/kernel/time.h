/*
 * include/uapi/kernel/time.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _UAPI_KERNEL_TIME_H
#define _UAPI_KERNEL_TIME_H

#include <asm/posix_types.h>

struct timespec {
	time_t tv_sec;  /* seconds */
	long   tv_nsec; /* nanoseconds */
};

struct itimerspec {
	struct timespec it_interval; /* timer period */
	struct timespec it_value;    /* timer expiration */
};

#define CLOCK_REALTIME	0
#define CLOCK_MONOTONIC	1

#endif /* !_UAPI_KERNEL_TIME_H */
