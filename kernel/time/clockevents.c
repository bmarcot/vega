/*
 * kernel/time/clockevents.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/time/clockevents.h>

#include <asm/ktime.h>

/* int clockevents_init_device(struct clock_event_device *dev) */
/* { */
/* 	return 0; */
/* } */

int clockevents_program_event(struct clock_event_device *dev, ktime_t expires)
{
	if (expires < 0)
		return -1;
	dev->next_event = expires;

	return dev->set_next_ktime(expires, dev);
}
