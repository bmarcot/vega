/*
 * include/uapi/kernel/resource.h
 *
 * Copyright (c) 2016-2017 Baruch Marcot
 *
 */

#ifndef _UAPI_KERNEL_RESOURCE_H
#define _UAPI_KERNEL_RESOURCE_H

#include <asm/posix_types.h>

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

struct rlimit {
	rlim_t rlim_cur;  /* Soft limit */
	rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) */
};

#define RLIMIT_STACK 0

#endif /* !_UAPI_KERNEL_RESOURCE_H */
