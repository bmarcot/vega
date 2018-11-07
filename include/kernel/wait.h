/*
 * include/kernel/wait.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_WAIT_H
#define _KERNEL_WAIT_H

#include <kernel/list.h>
#include <kernel/sched.h>

// move to another function default_wake_up()..
#include <asm/current.h>

struct wq_entry {
	struct task_struct	*tsk;
	struct list_head	list;
};

#define wait_event(wq_head, condition)				\
	({							\
		struct wq_entry __wq_entry;			\
		list_add(&__wq_entry.list, wq_head);		\
		for (;;) {					\
			if (condition)				\
				break;				\
			sched_dequeue(current);			\
			current->state = TASK_UNINTERRUPTIBLE;	\
			__wq_entry.tsk = current;		\
			schedule();				\
		}						\
		list_del(&__wq_entry.list);			\
		0;						\
	})

int wake_up(struct list_head *wq_head, int nr);

#endif /* !_KERNEL_WAIT_H */
