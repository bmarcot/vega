/*
 * include/kernel/time/clockevents.h
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#ifndef _KERNEL_TIME_CLOCKEVENTS_H
#define _KERNEL_TIME_CLOCKEVENTS_H

#include <kernel/types.h>

#include <asm/ktime.h>

enum clock_event_state {
	CLOCK_EVT_STATE_DETACHED,
	CLOCK_EVT_STATE_SHUTDOWN,
	CLOCK_EVT_STATE_PERIODIC,
	CLOCK_EVT_STATE_ONESHOT,
	CLOCK_EVT_STATE_ONESHOT_STOPPED,
};

#define CLOCK_EVT_FEAT_PERIODIC	0x000001
#define CLOCK_EVT_FEAT_ONESHOT	0x000002

struct clock_event_device {
	void	(*event_handler) (struct clock_event_device *);
	int	(*set_next_ktime) (ktime_t, struct clock_event_device *);

	const char		*name;
	ktime_t			next_event;
	unsigned int		features;
	enum clock_event_state	state_use_accessors;
	int			irq;
	struct list_head	list;

	int	(*set_state_periodic) (struct clock_event_device *);
	int	(*set_state_oneshot) (struct clock_event_device *);
	int	(*set_state_oneshot_stopped) (struct clock_event_device *);
	int	(*set_state_shutdown) (struct clock_event_device *);
	int	(*tick_resume) (struct clock_event_device *);

	/*
	 * For power-saving purpose we don't have a clocksource that could be
	 * use to order clockevents. Instead, we have to keep a relative
	 * clocksource based on the clock event device itself. This function
	 * returns the number of micrtoseconds since last clockevent expired.
	 */
	ktime_t	(*read_elapsed) (struct clock_event_device *);
};

/* Helpers to verify state of a clockevent device */
static inline int clockevent_state_detached(struct clock_event_device *dev)
{
	return dev->state_use_accessors == CLOCK_EVT_STATE_DETACHED;
}

static inline int clockevent_state_shutdown(struct clock_event_device *dev)
{
	return dev->state_use_accessors == CLOCK_EVT_STATE_SHUTDOWN;
}

static inline int clockevent_state_periodic(struct clock_event_device *dev)
{
	return dev->state_use_accessors == CLOCK_EVT_STATE_PERIODIC;
}

static inline int clockevent_state_oneshot(struct clock_event_device *dev)
{
	return dev->state_use_accessors == CLOCK_EVT_STATE_ONESHOT;
}

static inline int clockevent_state_oneshot_stopped(struct clock_event_device *dev)
{
	return dev->state_use_accessors == CLOCK_EVT_STATE_ONESHOT_STOPPED;
}

static inline void
clockevent_set_event_handler(struct clock_event_device *dev,
			void (*event_handler) (struct clock_event_device *))
{
	dev->event_handler = event_handler;
}

static inline enum clock_event_state
clockevent_get_state(struct clock_event_device *dev)
{
	return dev->state_use_accessors;
}

static inline void clockevent_set_state(struct clock_event_device *dev,
					enum clock_event_state state)
{
	dev->state_use_accessors = state;
}

static inline ktime_t clockevents_read_elapsed(struct clock_event_device *dev)
{
	return dev->read_elapsed(dev);
}

int clockevents_register_device(struct clock_event_device *dev);
struct clock_event_device *clockevents_get_device(const char *name);
void clockevents_list_devices(void);
int clockevents_program_event(struct clock_event_device *dev, ktime_t expires);
void clockevents_switch_state(struct clock_event_device *dev,
			enum clock_event_state state);
void clockevents_shutdown(struct clock_event_device *dev);

#endif /* !_KERNEL_TIME_CLOCKEVENTS_H */
