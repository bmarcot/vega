/*
 * kernel/timercore.c
 *
 * Copyright (C) 2016 Benoit Marcot
 *
 */

#include <errno.h>
#include <stddef.h> //FIXME: NULL is part of stdlib.h
#include <stdlib.h>
#include <time.h>

#include <kernel/errno-base.h>
#include <kernel/time.h>

static struct timer_operations *timer_operations;

void config_timer_operations(struct timer_operations *tops)
{
	timer_operations = tops;
}

// timer_alloc(struct vnode *device_root)
// timer_alloc("/dev/timers/systick")
// timer_alloc("/dev/timers/nrf_timers")
// timer_alloc("/dev/timers/nrf_timers/0")
struct timer_info *timer_alloc(void)
{
	struct timer_info *timer;

	timer = malloc(sizeof(struct timer_info));
	if (timer == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	timer->tops = timer_operations; //FIXME: see above, get from vnode or device
	if (timer->tops->timer_alloc(timer)) {
		free(timer);
		return NULL;
	}

	return timer;
}

int timer_configure(struct timer_info *timer,
		void (*callback)(struct timer_info *self))
{
	timer->callback = callback;

	return timer->tops->timer_configure(timer, callback);
}

int timer_set(struct timer_info *timer, const struct itimerspec *value)
{
	timer->running = 1;
	return timer->tops->timer_set(timer, value);
}

int timer_cancel(struct timer_info *timer)
{
	return timer->tops->timer_cancel(timer);
}

int timer_free(struct timer_info *timer)
{
	if (timer->running)
		timer->tops->timer_cancel(timer);
	timer->tops->timer_free(timer);
	free(timer);

	return 0;
}

void timer_expire_callback(struct timer_info *timer)
{
	if (timer->callback)
		timer->callback(timer);
}
