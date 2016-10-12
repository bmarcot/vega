/*
 * include/kernel/time.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_TIME_H
#define KERNEL_TIME_H

#include <sys/types.h>
#include "linux/list.h"

struct thread_info;

enum timer_type {
	TT_SLEEP,
	TT_TIMER
};

struct timer {
	timer_t timer_id; //FIXME: harvest storage, timer_id + timer_type
	struct thread_info *owner;
	u32 expire_clocktime;
	struct list_head list;  /* unordered list */
	enum timer_type timer_type;
	void *timer_data;
	char __timer_data[0];
};

struct timer *find_timer_by_id(timer_t timer_id, struct list_head *timer_list);
int reserve_timer_id(timer_t *timerid);

#endif /* !KERNEL_TIME_H */
