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

void task_exit(struct task_struct *task)
{
	// this is called after last thread has exited, or when the
	// task is killed
	free(task);
}

struct task_struct *current_task_struct(void)
{
	CURRENT_THREAD_INFO(curr_thread);

	return curr_thread->ti_struct; //FIXME: current->task
}

int sys_getpid(void)
{
	CURRENT_TASK_STRUCT(curr_task);

	return curr_task->pid;
}

pid_t do_fork(void)
{
	/* create a new child process */
	/* struct task_struct *child = ...; */
	/* if (child == NULL) */
	/* 	return -1; */

	/* sync thread info, update current thread SP_process */
	CURRENT_THREAD_INFO(parent_thread);
	parent_thread->thread_ctx.sp = __get_PSP();

	/* add a thread to child process */
	struct thread_info *child_thread =
		thread_clone(parent_thread, 0);
	sched_enqueue(child_thread);

	return child_thread->ti_struct->pid;
}

pid_t sys_fork(void)
{
	return do_fork();
}
