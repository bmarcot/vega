/*
 * ../nrf52/timer.h
 *
 * Copyright (C) 2016 Benoit Marcot
 */

#ifndef NRF52_TIMER_H
#define NRF52_TIMER_H

#define NRF52_TIMERS 5 // take from cmsis instead

struct nrf52_timer {
	int flags; // ONE_SHOT, PERIODIC, ...
	void (*isr_callback)(int timerid);
	int running;
};

struct nrf52_timer_info {
	NRF_TIMER_Type *nrf_timer;
	unsigned int irq_no;
	void (*isr_entry)(void);
};

//XXX: driver interface to hrtimer_xxx()?
struct nrf52_timer *alloc_timer(void);
void configure_timer(struct nrf52_timer *timer, void (*isr_callback)(int));
void set_timer(struct nrf52_timer *timer, unsigned int usec);
void cancel_timer();
void free_timer(struct nrf52_timer *timer);
void timer_init(void);

#endif /* !NRF52_TIMER_H */
