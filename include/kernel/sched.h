/*
 * include/kernel/sched.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _KERNEL_SCHED_H
#define _KERNEL_SCHED_H

#include <sys/types.h>

#include <arch/thread_info.h>

#include "linux/list.h"

#define FILE_MAX 8

struct file;

struct task_struct {
	union thread_union *stack; /* the kernel stack */

	int                ti_priority;
	int                ti_id;
	int                ti_state;
	int                ti_stacksize; /* thread stack's size */

	struct list_head   ti_list; /* global list of threads */
	struct list_head   ti_q;    /* sched runq, mutex waitq, thread joinq */

	/* http://www.domaigne.com/blog/computing/joinable-and-detached-threads/ */
	void               *ti_retval;
	int                ti_detached;
	int                ti_joinable;
	struct thread_info *ti_joining;

	/* Pointer to mutually exclusive data: the mutex the thread is blocking
	 * on, the exit value when thread is not yet joined, etc. */
	void               *ti_private;

	/* old task_info struct */
	pid_t            pid;
	unsigned long    filemap;
	struct file      *filetable[FILE_MAX];
};

#define THREAD_SIZE 512

union thread_union {
	struct thread_info thread_info;
	unsigned long      stack[THREAD_SIZE / sizeof(long)];
};

//FIXME: This should go to arch/arm/asm/current.h
static inline struct task_struct *get_current(void)
{
	return current_thread_info()->task;
}

#endif /* !_KERNEL_SCHED_H */