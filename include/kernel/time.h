/*
 * include/kernel/time.h
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include <sys/types.h>
#include "linux/list.h"

struct timer_info;
struct itimerspec;

enum timer_type { ONESHOT_TIMER, INTERVAL_TIMER };

struct timer_operations {
	int (*timer_alloc)(struct timer_info *timer/* , int flags */);
	int (*timer_configure)(struct timer_info *timer,
			void (*callback)(struct timer_info *self));
	int (*timer_set)(struct timer_info *timer,
			const struct timespec *value, enum timer_type type);
	int (*timer_get)(struct timer_info *timer, struct itimerspec *value);
	int (*timer_cancel)(struct timer_info *timer);
	int (*timer_free)(struct timer_info *timer);
};

struct timer_info {
	timer_t            id;
	u32                flags;
	int                running;
	int                it_link; /* link to an interval timer */
	void               (*callback)(struct timer_info *self);
	struct thread_info *owner;
	struct itimerspec  value;
	struct list_head   list;
	const struct timer_operations *tops;

	//XXX: what goes in there?
	//struct device *dev;
	void *dev;
	void *priv;
};

struct timer_info *timer_alloc(void);
int timer_configure(struct timer_info *timer,
		void (*callback)(struct timer_info *self));
int timer_set(struct timer_info *timer, const struct timespec *value,
	enum timer_type type);
int timer_get(struct timer_info *timer, struct itimerspec *value);
int timer_cancel(struct timer_info *timer);
int timer_free(struct timer_info *timer);

void timer_expire_callback(struct timer_info *timer);

#endif /* !_KERNEL_TIME_H */
