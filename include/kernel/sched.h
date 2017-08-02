/*
 * include/kernel/sched.h
 *
 * Copyright (c) 2016-2017 Baruch Marcot
 */

#ifndef _KERNEL_SCHED_H
#define _KERNEL_SCHED_H

#include <kernel/types.h>

#include <asm/thread_info.h>

/* 0 <= PRI_MAX <= PRI_MIN */
#define PRI_MAX    0
#define PRI_MIN    31

struct task_struct;

int sched_init(void);
int sched_enqueue(struct task_struct *task);
int sched_dequeue(struct task_struct *task);
int schedule(void);

#define TASK_RUNNING         0
#define TASK_INTERRUPTIBLE   0x1
#define TASK_UNINTERRUPTIBLE 0x2
#define TASK_NEW             0x4
#define EXIT_ZOMBIE          0x8
#define EXIT_DEAD            0x16

#define FILE_MAX 8

struct file;

struct task_struct {
#ifdef CONFIG_THREAD_INFO_IN_TASK
	struct thread_info thread_info;
#else
	struct thread_info *thread_info;
#endif
	void               *stack;
	int                state;
	int                flags;
	int                exit_code;
	int                prio;
	int                pid;
	int                sig;
	struct list_head   list;    /* global list of tasks */

	struct list_head   ti_q;    /* sched runqueue */

	/* old task_info struct */
	unsigned long    filemap;
	struct file      *filetable[FILE_MAX];
};

static inline struct thread_info *task_thread_info(struct task_struct *task)
{
#ifdef CONFIG_THREAD_INFO_IN_TASK
	return &task->thread_info;
#else
	return task->thread_info;
#endif
}

int init_task(struct task_struct *task);
struct task_struct *clone_task(int (*fn)(void *), void *child_stack,
			int flags, void *arg);

int arch_thread_setup(struct task_struct *task, void *start_routine,
		void *arg, void *stack_start);

#endif /* !_KERNEL_SCHED_H */
