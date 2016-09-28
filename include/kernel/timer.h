/*
 * include/kernel/timer.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

#include <sys/types.h>

#include <kernel/thread.h>

#include "linux/types.h"
#include "linux/list.h"

enum timer_type {
	TT_SLEEP,
	TT_TIMER
};

struct timer {
	struct thread_info *owner;
	u32 expire_clocktime;
	struct list_head list;  /* unordered list */
	enum timer_type timer_type;
};

/* system interfaces */
int timer_create(unsigned int msecs);

#endif /* !KERNEL_TIMER_H */
