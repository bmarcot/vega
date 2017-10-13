/*
 * kernel/task.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/list.h>
#include <kernel/sched.h>
#include <kernel/stddef.h>

static struct list_head tasks;

int init_task(struct task_struct *task)
{
	static int tgid = 1;

	task->prio = PRI_MIN;
	task->state = TASK_NEW;
	task->stack = &task->thread_info;
	task->tgid = tgid++;
	task->filemap = 0;
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;
	list_add(&task->list, &tasks);

	return 0;
}
