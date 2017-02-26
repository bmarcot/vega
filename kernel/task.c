/*
 * kernel/task.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/task.h>

extern struct task_info top_task;

int sys_getpid(void)
{
	return top_task.pid;
}
