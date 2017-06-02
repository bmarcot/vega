/*
 * include/kernel/sched.h
 *
 * Copyright (c) 2016-2017 Baruch Marcot
 */

#ifndef _KERNEL_SCHED_H
#define _KERNEL_SCHED_H

#include <sys/types.h>

#include <arch/thread_info.h>

#include "linux/list.h"

/*
 * Scheduling
 */

/* 0 <= PRI_MAX <= PRI_MIN */
#define PRI_MAX    0
#define PRI_MIN    31

#define SCHED_OPT_NONE            0
#define SCHED_OPT_RESTORE_ONLY    1
#define SCHED_OPT_RESET           2

struct task_struct;

int sched_init(void);
int sched_enqueue(struct task_struct *task);
int sched_dequeue(struct task_struct *task);
int sched_elect(int flags);

/*
 * Tasking
 */

#define FILE_MAX 8

struct file;

struct task_struct {
#ifdef CONFIG_THREAD_INFO_IN_TASK
	struct thread_info thread_info;
#else
	struct thread_info  *thread_info; /* the kernel stack */
#endif

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

//FIXME: This should go to arch/arm/asm/current.h
static inline struct task_struct *get_current(void)
{
	return current_thread_info()->task;
}

static inline struct thread_info *task_thread_info(struct task_struct *task)
{
	return task->thread_info;
}

//XXX: Will die...
static inline struct task_struct *TASK_STRUCT(struct thread_info *ti)
{
#ifdef CONFIG_THREAD_INFO_IN_TASK
	return (struct task_struct *)(ti + 1);
#else
	return ti->task;
#endif
}

#endif /* !_KERNEL_SCHED_H */
