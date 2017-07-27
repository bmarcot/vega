/*
 * include/kernel/time.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include <kernel/sched.h>
#include <kernel/types.h>

#include <uapi/kernel/signal.h>
#include <uapi/kernel/time.h>

struct timer_info;

enum timer_type { ONESHOT_TIMER, INTERVAL_TIMER };

struct timer_operations {
	int (*timer_alloc)(struct timer_info *timer/* , int flags */);
	int (*timer_free)(struct timer_info *timer);
	int (*timer_set)(struct timer_info *timer,
			const struct timespec *value);
	int (*timer_get)(struct timer_info *timer, struct itimerspec *value);
};

struct timer_info {
	timer_t            id;
	u32                flags;
	int                disarmed;
	enum timer_type    type;
	void               (*callback)(struct timer_info *self);
	struct task_struct *owner;
	struct itimerspec  value;
	struct sigevent    sigev;
	struct list_head   list;
	const struct timer_operations *tops;

	//XXX: what goes in there?
	//struct device *dev;
	void *dev;
};

struct timer_info *timer_alloc(void);
int timer_free(struct timer_info *timer);
int timer_set(struct timer_info *timer, const struct timespec *value);
int timer_get(struct timer_info *timer, struct itimerspec *value);

void timer_expire_callback(struct timer_info *timer);

#endif /* !_KERNEL_TIME_H */
