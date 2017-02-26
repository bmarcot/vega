/*
 * include/kernel/task.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <sys/types.h>

#include "linux/list.h"

struct task_info {
	pid_t            pid;
	struct list_head thread_head;
	struct list_head signal_head; /* list of installed handlers */
};

#endif /* !_KERNEL_TASK_H */
