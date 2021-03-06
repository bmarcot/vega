/*
 * kernel/time/clockevents.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clockevents.h>
#include <kernel/errno-base.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/string.h>

#include <asm/ktime.h>

static LIST_HEAD(clockevent_devices);

int clockevents_register_device(struct clock_event_device *dev)
{
	/* Initialize state to DETACHED */
	clockevent_set_state(dev, CLOCK_EVT_STATE_DETACHED);

	list_add_tail(&dev->list, &clockevent_devices);

	return 0;
}

struct clock_event_device *clockevents_get_device(const char *name)
{
	struct clock_event_device *dev;

	list_for_each_entry(dev, &clockevent_devices, list) {
		if (!strcmp(name, dev->name))
			return dev;
	}

	return NULL;
}

struct clock_event_device *clockevents_get_device2(dev_t devt)
{
	struct clock_event_device *dev;

	list_for_each_entry(dev, &clockevent_devices, list) {
		if (dev->char_dev.dev == devt)
			return dev;
	}

	return NULL;
}

void clockevents_list_devices(void)
{
	struct clock_event_device *dev;

	list_for_each_entry(dev, &clockevent_devices, list) {
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

static int __clockevents_switch_state(struct clock_event_device *dev,
				enum clock_event_state state)
{
	switch (state) {
	case CLOCK_EVT_STATE_DETACHED:
		/* The clockevent device is getting replaced. Shut it down. */

	case CLOCK_EVT_STATE_SHUTDOWN:
		if (dev->set_state_shutdown)
			return dev->set_state_shutdown(dev);
		return 0;

	case CLOCK_EVT_STATE_PERIODIC:
		if (!(dev->features & CLOCK_EVT_FEAT_PERIODIC))
			return -ENOSYS;
		if (dev->set_state_periodic)
			return dev->set_state_periodic(dev);
		return 0;

	case CLOCK_EVT_STATE_ONESHOT:
		if (!(dev->features & CLOCK_EVT_FEAT_ONESHOT))
			return -ENOSYS;
		if (dev->set_state_oneshot)
			return dev->set_state_oneshot(dev);
		return 0;

	case CLOCK_EVT_STATE_ONESHOT_STOPPED:
		if (!clockevent_state_oneshot(dev))
			return -EINVAL;
		if (dev->set_state_oneshot_stopped)
			return dev->set_state_oneshot_stopped(dev);
		else
			return -ENOSYS;

	default:
		return -ENOSYS;
	}
}

void clockevents_switch_state(struct clock_event_device *dev,
			enum clock_event_state state)
{
	if (clockevent_get_state(dev) != state) {
		if (__clockevents_switch_state(dev, state))
			return;
		clockevent_set_state(dev, state);
	}
}

void clockevents_shutdown(struct clock_event_device *dev)
{
	clockevents_switch_state(dev, CLOCK_EVT_STATE_SHUTDOWN);
	dev->next_event = KTIME_MAX;
}
