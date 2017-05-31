/*
 * include/kernel/task.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <sys/types.h>

#include <kernel/fs.h>

#include "linux/list.h"

#define PID_BASE 7000
#define PID_MAX  32768

struct task_struct *task_create(void *(*start_routine)(void *), void *arg);

#endif /* !_KERNEL_TASK_H */
