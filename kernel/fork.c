/*
 * kernel/fork.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <mm/page.h>

#include "linux/list.h"

int sys_getpid(void)
{
	return get_current()->pid;
}

pid_t do_fork(void)
{
	/* create a new child process */
	char *child_stack = alloc_pages(size_to_page_order(512));
	struct task_struct *child = clone_task(NULL, child_stack + 512, 0, NULL);
	if (child == NULL)
		return -1;
	// child->parent = get_current();
	sched_enqueue(child);

	return child->pid;
}

pid_t sys_fork(void)
{
	return do_fork();
}

#define THREAD_SIZE 512

struct task_struct *clone_task(int (*fn)(void *), void *child_stack,
			int flags, void *arg)
{
	struct task_struct *tsk;

	tsk = alloc_pages(size_to_page_order(THREAD_SIZE));
	if (!tsk)
		return NULL;
	init_task(tsk);
	tsk->flags = flags;
	arch_thread_setup(tsk, fn, arg, child_stack);

	return tsk;
}

int sys_clone(int (*fn)(void *), void *child_stack, int flags, void *arg)
{
	struct task_struct *tsk;

	tsk = clone_task(fn, child_stack, flags, arg);
	if (!tsk)
		return -1;
	sched_enqueue(tsk);

	return tsk->pid;
}
