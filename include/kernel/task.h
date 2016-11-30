/*
 * include/kernel/task.h
 *
 * Copyright (C) 2016 Benoit Marcot
 */

#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include "linux/list.h"

struct task_info {
	struct list_head signal_head; /* list of installed handlers */
};

#endif /* !_KERNEL_TASK_H */
