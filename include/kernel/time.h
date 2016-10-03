/*
 * include/kernel/time.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_TIME_H
#define KERNEL_TIME_H

#include <sys/types.h>

struct list_head;

struct timer *find_timer_by_id(timer_t timer_id, struct list_head *timer_list);
int reserve_timer_id(timer_t *timerid);

#endif /* !KERNEL_TIME_H */
