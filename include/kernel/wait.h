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

#define ERESTARTSYS 1

#define __wait_event(wq_head, condition, state)				\
	({								\
		struct wait_queue_entry __wq_entry;			\
		long __ret = 0;						\
		init_wait_queue_entry(&__wq_entry);			\
		for (;;) {						\
			long __int = prepare_to_wait_event(wq_head, &__wq_entry, state); \
			if (condition)					\
				break;					\
			if (__int) {					\
				__ret = __int;				\
				goto __out;				\
			}						\
			schedule();					\
		}							\
		finish_wait(&__wq_entry);				\
	__out:	__ret;							\
	})


#define wait_event_interruptible(wq_head, condition)			\
	({								\
		int __ret = 0;						\
		/* might_sleep(); */					\
		if (!(condition))					\
			__ret = __wait_event(wq_head, condition, TASK_INTERRUPTIBLE); \
		__ret;							\
	})

#define wait_event(wq_head, condition)					\
	({								\
		int __ret = 0;						\
		/* might_sleep(); */					\
		if (!(condition))					\
			__ret = __wait_event(wq_head, condition, TASK_UNINTERRUPTIBLE); \
		__ret;							\
	})

#endif /* !_KERNEL_WAIT_H */
