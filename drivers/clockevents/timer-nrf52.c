/*
 * drivers/clockevents/timer-nrf52.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/time/clockevents.h>

#include "platform.h"

#define NSEC_PER_USEC 1000l

struct nrf52_clockevent {
	struct clock_event_device	dev;
	NRF_TIMER_Type			*hw;
	u8				usec_per_tick;
	void				(*interrupt)();
};

static void nrf52_timer0_interrupt(void);
static void nrf52_timer1_interrupt(void);
static void nrf52_timer2_interrupt(void);
static void nrf52_timer3_interrupt(void);
static void nrf52_timer4_interrupt(void);

static int nrf52_clkevt_set_next_ktime(ktime_t expires,
				struct clock_event_device *dev)
{
	struct nrf52_clockevent *clkevt =
		container_of(dev, struct nrf52_clockevent, dev);
	NRF_TIMER_Type *timer = clkevt->hw;
	unsigned long usec = expires / (u64)NSEC_PER_USEC; // ktime_t is in nanoseconds

	/* reset timer */
	timer->TASKS_STOP = 1;
	timer->EVENTS_COMPARE[0] = 0;

	if (!usec)
		return 0;

	/* lookup tables */
	const u8 prescaler_tbl[] = {9, 4, 5, 4, 6, 4, 5, 4,
				    7, 4, 5, 4, 6, 4, 5, 4,
				    8, 4, 5, 4, 6, 4, 5, 4,
				    7, 4, 5, 4, 6, 4, 5, 4};
	const u8 tick_tbl[] = {1, 2, 4, 8, 16, 32}; /* microseconds per tick */
	const u8 bitmode_tbl[] = {TIMER_BITMODE_BITMODE_32Bit,
				  TIMER_BITMODE_BITMODE_24Bit,
				  TIMER_BITMODE_BITMODE_16Bit,
				  TIMER_BITMODE_BITMODE_08Bit};

	/* calculate the minimum number of comparisons */
	u8 prescaler = prescaler_tbl[usec % 32];
	u8 usec_per_tick = tick_tbl[prescaler - 4];
	u32 cc = usec / usec_per_tick;
	u8 bitmode = bitmode_tbl[__builtin_clz(cc) / 8];

	clkevt->usec_per_tick = usec_per_tick;
	timer->CC[0] = cc;
	timer->BITMODE = bitmode << TIMER_BITMODE_BITMODE_Pos;
	timer->PRESCALER = prescaler << TIMER_PRESCALER_PRESCALER_Pos;
	timer->TASKS_START = 1;

	return 0;
}

static int nrf52_clkevt_set_state_periodic(struct clock_event_device *dev)
{
	NRF_TIMER_Type *timer = container_of(dev, struct nrf52_clockevent, dev)->hw;

	timer->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;

	return 0;
}

static int nrf52_clkevt_set_state_oneshot(struct clock_event_device *dev)
{
	NRF_TIMER_Type *timer = container_of(dev, struct nrf52_clockevent, dev)->hw;

	timer->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos)
		| (TIMER_SHORTS_COMPARE0_STOP_Enabled << TIMER_SHORTS_COMPARE0_STOP_Pos);

	return 0;
}

static ktime_t nrf52_clkevt_read_elapsed(struct clock_event_device *dev)
{
	struct nrf52_clockevent *clkevt =
		container_of(dev, struct nrf52_clockevent, dev);
	NRF_TIMER_Type *timer = clkevt->hw;

	timer->TASKS_CAPTURE[0] = 1;

	return (ktime_t)timer->CC[0] * clkevt->usec_per_tick * NSEC_PER_USEC;
}

#define NRF52_TIMER_DEFINE(id) {					\
	.dev = {							\
		.set_next_ktime = nrf52_clkevt_set_next_ktime,		\
		.name = "nrf52-timer"#id,				\
		.features = CLOCK_EVT_FEAT_ONESHOT | CLOCK_EVT_FEAT_PERIODIC, \
		.irq = TIMER##id##_IRQn,				\
		.set_state_periodic = nrf52_clkevt_set_state_periodic,	\
		.set_state_oneshot = nrf52_clkevt_set_state_oneshot,	\
		.read_elapsed = nrf52_clkevt_read_elapsed,		\
	},								\
	.hw = NRF_TIMER##id,						\
	.interrupt = nrf52_timer##id##_interrupt,			\
}

static struct nrf52_clockevent nrf52_clockevent[] = {
	NRF52_TIMER_DEFINE(0),
	NRF52_TIMER_DEFINE(1),
	NRF52_TIMER_DEFINE(2),
	NRF52_TIMER_DEFINE(3),
	NRF52_TIMER_DEFINE(4),
};

static void nrf52_timer_interrupt(int timerid)
{
	nrf52_clockevent[timerid].hw->EVENTS_COMPARE[0] = 0;

	struct clock_event_device *clkevt = &nrf52_clockevent[timerid].dev;
	if (clkevt->event_handler)
		clkevt->event_handler(clkevt);
}

static void nrf52_timer0_interrupt(void)
{
	nrf52_timer_interrupt(0);
}

static void nrf52_timer1_interrupt(void)
{
	nrf52_timer_interrupt(1);
}

static void nrf52_timer2_interrupt(void)
{
	nrf52_timer_interrupt(2);
}

static void nrf52_timer3_interrupt(void)
{
	nrf52_timer_interrupt(3);
}

static void nrf52_timer4_interrupt(void)
{
	nrf52_timer_interrupt(4);
}

static int nrf52_timer_init(struct nrf52_clockevent *clkevt)
{
	clkevt->hw->MODE = TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
	clkevt->hw->INTENSET = TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos;
	irq_attach(clkevt->dev.irq, clkevt->interrupt);
	NVIC_EnableIRQ(clkevt->dev.irq);
	if (clockevents_register_device(&clkevt->dev))
		return -1;

	return 0;
}

int init_nrf52_timers(void)
{
	for (int i = 0; i < TIMER_COUNT; i++) {
		if (nrf52_timer_init(&nrf52_clockevent[i]))
			return -1;
	}

	return 0;
}
