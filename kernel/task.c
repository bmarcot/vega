/*
 * kernel/task.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/sched.h>
#include <kernel/thread.h>

static struct list_head tasks;

int init_task(struct task_struct *task)
{
	static int pid = 9000;

	task->prio = PRI_MIN;
	task->ti_state = THREAD_STATE_NEW;

	task->stack = &task->thread_info;
	task->pid = pid++;
	task->filemap = 0;
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;

	list_add(&task->ti_list, &tasks);

	return 0;
}
