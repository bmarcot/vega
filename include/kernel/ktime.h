/*
 * include/kernel/ktime.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_KTIME_H
#define _KERNEL_KTIME_H

#include <kernel/types.h>

#include <uapi/kernel/time.h>

#include <asm/ktime.h>

static inline ktime_t ktime_set(const s64 secs, const unsigned long nsecs)
{
#define NSEC_PER_SEC 1000000000l
	return secs * NSEC_PER_SEC + (s64)nsecs;
}

static inline ktime_t timespec_to_ktime(struct timespec ts)
{
	return ktime_set(ts.tv_sec, ts.tv_nsec);
}

static inline struct timespec ktime_to_timespec(ktime_t ktime)
{
	struct timespec ts = {
		.tv_sec = ktime / NSEC_PER_SEC,
		.tv_nsec = ktime % NSEC_PER_SEC,
	};

	return ts;
}

#endif /* !_KERNEL_KTIME_H */
