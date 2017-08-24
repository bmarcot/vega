/*
 * kernel/time/clockevents.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <string.h>

#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/time/clockevents.h>

#include <asm/ktime.h>

LIST_HEAD(clock_event_devs);

int clockevents_register_device(struct clock_event_device *dev)
{
	list_add(&dev->list, &clock_event_devs);

	return 0;
}

struct clock_event_device *clockevents_get_device(const char *name)
{
	struct clock_event_device *dev;

	list_for_each_entry(dev, &clock_event_devs, list) {
		if (!strcmp(name, dev->name))
			return dev;
	}

	return NULL;
}

void clockevents_list_devices(void)
{
	struct clock_event_device *dev;

	list_for_each_entry(dev, &clock_event_devs, list) {
		printk("clock_event_device: %s\n", dev->name);
	}
}

int clockevents_program_event(struct clock_event_device *dev, ktime_t expires)
{
	if (expires < 0)
		return -1;
	dev->next_event = expires;

	return dev->set_next_ktime(expires, dev);
}
