/*
 * include/kernel/wait.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_WAIT_H
#define _KERNEL_WAIT_H

#include <kernel/list.h>
#include <kernel/sched.h>

struct wait_queue_entry;

typedef int (*wait_queue_func_t)(struct wait_queue_entry *wq_entry);

struct wait_queue_entry {
	void			*private;
	wait_queue_func_t	func;
	struct list_head	list;
};

int wake_up(struct list_head *wq_head, int nr);

void init_wait_queue_entry(struct wait_queue_entry *wq_entry);
int prepare_to_wait_event(struct list_head *wq_head,
			struct wait_queue_entry *wq_entry, int state);
void finish_wait(struct wait_queue_entry *wq_entry);

#define wait_event(wq_head, condition)				\
	({							\
		struct wait_queue_entry __wq_entry;		\
		long __ret = 0;					\
		init_wait_queue_entry(&__wq_entry);		\
		for (;;) {					\
			prepare_to_wait_event(wq_head, &__wq_entry, 0);	\
			if (condition)				\
				break;				\
			schedule();				\
		}						\
		finish_wait(&__wq_entry);			\
		__ret;						\
	})

#endif /* !_KERNEL_WAIT_H */
