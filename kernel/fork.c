/*
 * kernel/fork.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/sched.h>
#include <kernel/thread.h>

#include "linux/list.h"

int sys_getpid(void)
{
	return get_current()->pid;
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
	struct thread_info *child_thread = thread_clone(parent_thread, 0);
	sched_enqueue(TASK_STRUCT(child_thread));

	return TASK_STRUCT(child_thread)->pid;
}

pid_t sys_fork(void)
{
	return do_fork();
}