/*
 * include/kernel/time.h
 *
 * Copyright (C) 2016 Benoit Marcot
 */

#ifndef KERNEL_TIME_H
#define KERNEL_TIME_H

#include <sys/types.h>
#include "linux/list.h"

struct timer_info;
struct itimerspec;

struct timer_operations {
	int (*timer_alloc)(struct timer_info *timer/* , int flags */);
	int (*timer_configure)(struct timer_info *timer,
			void (*callback)(struct timer_info *self));
	int (*timer_set)(struct timer_info *timer, const struct itimerspec *value);
	int (*timer_get)(struct timer_info *timer, struct itimerspec *value);
	int (*timer_cancel)(struct timer_info *timer);
	int (*timer_free)(struct timer_info *timer);
};

struct timer_info {
	u32 flags;
	int running;
	struct timer_operations *tops;
	void (*callback)(struct timer_info *self);
	//struct thread_info *owner;
	struct list_head list;
	timer_t id;
	struct itimerspec value;

	//XXX: what goes in there?
	//struct device *dev;
	void *dev;
	void *priv;
};

struct timer_info *timer_alloc(void);
int timer_configure(struct timer_info *timer,
		void (*callback)(struct timer_info *self));
int timer_set(struct timer_info *timer, const struct itimerspec *value);
int timer_get(struct timer_info *timer, struct itimerspec *value);
int timer_cancel(struct timer_info *timer);
int timer_free(struct timer_info *timer);

void timer_expire_callback(struct timer_info *timer);

#endif /* !KERNEL_TIME_H */
