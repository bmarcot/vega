/*
 * include/kernel/timer.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

//FIXME: move to time.h

#include <sys/types.h>

#include <kernel/thread.h>

#include "linux/types.h"
#include "linux/list.h"

enum timer_type {
	TT_SLEEP,
	TT_TIMER
};

struct timer {
	timer_t timer_id; // harvest storage! timer_id + timer type
	struct thread_info *owner;
	u32 expire_clocktime;  //FIXME: rename to 'timeout'
	struct list_head list;  /* unordered list */
	enum timer_type timer_type;
	void *timer_data;
	char __timer_data[0];
};

/* system interfaces */
int timer_create(unsigned int msecs);

#endif /* !KERNEL_TIMER_H */
