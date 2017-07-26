/*
 * kernel/task.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stddef.h>

#include <kernel/list.h>
#include <kernel/sched.h>
#include <kernel/types.h>

static struct list_head tasks;

int init_task(struct task_struct *task)
{
	static int pid = 9000;

	task->prio = PRI_MIN;
	task->state = TASK_NEW;
	task->stack = &task->thread_info;
	task->pid = pid++;
	task->filemap = 0;
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;
	list_add(&task->list, &tasks);

	return 0;
}
