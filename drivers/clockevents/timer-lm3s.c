/*
 * drivers/clockevents/timer-lm3s.c
 *
 * Copyright (c) 2017-2019 Benoit Marcot
 */

#include <kernel/clockevents.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/time.h>

#include "platform.h"

struct lm3s_clockevent {
	struct clock_event_device	dev;
	TIMER_Type			*hw;
};

#ifdef QEMU
#include <kernel/clocksource.h>
u64 systick_read(struct clocksource *cs);
static u64 set_next;
#endif /* QEMU */

static int lm3s_clkevt_set_next_ktime(ktime_t expires,
				struct clock_event_device *dev)
{
	TIMER_Type *hw = container_of(dev, struct lm3s_clockevent, dev)->hw;
	u32 ticks_per_usec = SystemFrequency / USEC_PER_SEC;

	/* Ensure the timer is disabled before making any changes */
	hw->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;

	/* Clear any pending interrupts */
	hw->ICR |= GPTM_GPTMICR_TATOCINT_Msk;
	NVIC_ClearPendingIRQ(dev->irq);

	if (!expires) {
		hw->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;
		hw->ICR |= GPTM_GPTMICR_TATOCINT_Msk;
	}

	/*
	 * We handle granularity of a microsecond, everything smaller is
	 * just noise. SystemFrequency must be in the MHz range.
	 */
	hw->TAILR = (expires / (u64)NSEC_PER_USEC) * ticks_per_usec;

#ifdef QEMU
	/* Record the time when timer was started */
	set_next = systick_read(NULL);
#endif

	/* Enable the timer and start counting */
	hw->CTL |= (GPTM_GPTMCTL_TAEN_Enabled << GPTM_GPTMCTL_TAEN_Pos);

	return 0;
}

static int lm3s_clkevt_set_state_oneshot(struct clock_event_device *dev)
{
	TIMER_Type *hw = container_of(dev, struct lm3s_clockevent, dev)->hw;

	/* Ensure the timer is disabled before making any changes */
	hw->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;

	hw->CFG = 0;
	hw->TAMR = GPTM_GPTMTAMR_TAMR_OneShot << GPTM_GPTMTAMR_TAMR_Pos;

	return 0;
}

static int lm3s_clkevt_set_state_shutdown(struct clock_event_device *dev)
{
	TIMER_Type *hw = container_of(dev, struct lm3s_clockevent, dev)->hw;

	/* Ensure the timer is disabled before making any changes */
	hw->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;

	/* Clear any pending interrupts */
	hw->ICR |= GPTM_GPTMICR_TATOCINT_Msk;
	NVIC_ClearPendingIRQ(dev->irq);

	return 0;
}

static ktime_t lm3s_clkevt_read_elapsed(struct clock_event_device *dev)
{
#ifdef QEMU
	if (!set_next)
		return 0;

	/* Timer0 and SysTick share the same internal clock source (XTALI) */
	return clocksource_cyc2ns(systick_read(NULL) - set_next, dev->mult,
				dev->shift);
#else
#warning "Missing implementation"
#endif
}

/* use general purpose timer 0 as clock event device */
static struct lm3s_clockevent lm3s_clockevent = {
	.dev = {
		.set_next_ktime = lm3s_clkevt_set_next_ktime,
		.name = "lm3s-timer0",
		.mult = 174762667,
		.shift = 21,
		.features = CLOCK_EVT_FEAT_ONESHOT,
		.irq = Timer0A_IRQn,
		.set_state_oneshot = lm3s_clkevt_set_state_oneshot,
		.set_state_shutdown = lm3s_clkevt_set_state_shutdown,

		.read_elapsed = lm3s_clkevt_read_elapsed,
		.char_dev = {0},
	},
	.hw = TIMER0,
};

void lm3s_timer_interrupt(void)
{
#ifdef QEMU
	set_next = 0;
#endif

	/* clear the interrupt */
	lm3s_clockevent.hw->ICR |=
		(GPTM_GPTMICR_TATOCINT_Cleared << GPTM_GPTMICR_TATOCINT_Pos);

	if (lm3s_clockevent.dev.event_handler)
		lm3s_clockevent.dev.event_handler(&lm3s_clockevent.dev);
}

int clockchar_dev_register(struct clock_event_device *, int);

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
	if (clockevents_register_device(&lm3s_clockevent.dev))
		return -1;

	clockchar_dev_register(&lm3s_clockevent.dev, 0);

	return 0;
}
