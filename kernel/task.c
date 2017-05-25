/*
 * kernel/task.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <stdlib.h>

#include <kernel/scheduler.h>
#include <kernel/task.h>
#include <kernel/thread.h>

#include "linux/list.h"

static LIST_HEAD(task_head);

static pid_t alloc_pid()
{
	static pid_t pid = 7000;
	pid_t retpid;

	retpid = pid;
	pid++;

	return retpid;
}

struct task_info *task_init(struct task_info *task)
{
	task->pid = alloc_pid();
	task->filemap = 0;
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;
	INIT_LIST_HEAD(&task->thread_head);
	INIT_LIST_HEAD(&task->signal_head);
	list_add(&task->list, &task_head);

	return task;
}

void task_exit(struct task_info *task)
{
	// this is called after last thread has exited, or when the
	// task is killed
	list_del(&task->list);
	free(task);
}

struct task_info *current_task_info(void)
{
	CURRENT_THREAD_INFO(curr_thread);

	return curr_thread->ti_task;
}

int sys_getpid(void)
{
	CURRENT_TASK_INFO(curr_task);

	return curr_task->pid;
}

pid_t do_fork(void)
{
	/* create a new child process */
	struct task_info *child = malloc(sizeof(struct task_info));
	if (child == NULL)
		return -1;
	task_init(child);

	/* sync thread info, update current thread SP_process */
	CURRENT_THREAD_INFO(parent_thread);
	parent_thread->ti_mach.mi_psp = __get_PSP();

	/* add a thread to child process */
	struct thread_info *child_thread =
		thread_clone(parent_thread, 0, child);
	sched_enqueue(child_thread);

	return child->pid;
}

pid_t sys_fork(void)
{
	return do_fork();
}
