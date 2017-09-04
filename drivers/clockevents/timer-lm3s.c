/*
 * drivers/clockevents/timer-lm3s.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/kernel.h>
#include <kernel/time/clockevents.h>

#include "platform.h"

#define USEC_PER_SEC  1000000l
#define NSEC_PER_USEC 1000l

struct lm3s_clockevent {
	struct clock_event_device dev;
	TIMER_Type *hw;
};

static int lm3s_clkevt_set_next_ktime(ktime_t expires,
				struct clock_event_device *dev)
{
	TIMER_Type *hw = container_of(dev, struct lm3s_clockevent, dev)->hw;
	u32 ticks_per_usec = SystemFrequency / USEC_PER_SEC;

	/*
	 * We handle granularity of a microsecond, everything smaller is
	 * just noise. SystemFrequency must be in the MHz range.
	 */
	hw->TAILR = (expires / (u64)NSEC_PER_USEC) * ticks_per_usec;
	hw->CTL |= (GPTM_GPTMCTL_TAEN_Enabled << GPTM_GPTMCTL_TAEN_Pos);

	return 0;
}

static int lm3s_clkevt_set_state_oneshot(struct clock_event_device *dev)
{
	TIMER_Type *hw = container_of(dev, struct lm3s_clockevent, dev)->hw;

	/* disable Timer0 and reconfigure */
	hw->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;
	hw->CFG = 0;
	hw->TAMR = GPTM_GPTMTAMR_TAMR_OneShot << GPTM_GPTMTAMR_TAMR_Pos;

	return 0;
}

/* use general purpose timer 0 as clock event device */
static struct lm3s_clockevent lm3s_clockevent = {
	.dev = {
		.name = "lm3s-timer0",
		.set_next_ktime = lm3s_clkevt_set_next_ktime,
		.set_state_oneshot = lm3s_clkevt_set_state_oneshot,
		.features = CLOCK_EVT_FEAT_ONESHOT,
		.irq = Timer0A_IRQn,
	},
	.hw = TIMER0,
};

void lm3s_timer_interrupt(void)
{
	/* clear the interrupt */
	lm3s_clockevent.hw->ICR |=
		(GPTM_GPTMICR_TATOCINT_Cleared << GPTM_GPTMICR_TATOCINT_Pos);

	if (lm3s_clockevent.dev.event_handler)
		lm3s_clockevent.dev.event_handler(&lm3s_clockevent.dev);
}

int lm3s_timer_init(/* struct device_node *node */)
{
	/*
	 * To use the general-purpose timers, the peripheral clock must
	 * be enabled by setting the TIMER0, TIMER1, TIMER2, and TIMER3
	 * bits in the RCGC1 register.
	 */

	irq_attach(lm3s_clockevent.dev.irq, lm3s_timer_interrupt);
	TIMER0->IMR |= 1 << GPTM_GPTMIMR_TATOIM_Pos;
	NVIC_EnableIRQ(lm3s_clockevent.dev.irq);
	lm3s_clkevt_set_state_oneshot(&lm3s_clockevent.dev);
	if (clockevents_register_device(&lm3s_clockevent.dev))
		return -1;

	return 0;
}
