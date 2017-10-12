/*
 * include/kernel/resource.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_RESOURCE_H
#define _KERNEL_RESOURCE_H

#include <uapi/kernel/resource.h>

int do_getrlimit(int resource, struct rlimit *rlim);

#endif /* !_KERNEL_RESOURCE_H */

