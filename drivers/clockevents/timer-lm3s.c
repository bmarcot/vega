/*
 * drivers/clockevents/timer-lm3s.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/clockevents.h>
#include <kernel/clocksource.h> // For QEMU
#include <kernel/kernel.h>
#include <kernel/irq.h>

#include "platform.h"

#define USEC_PER_SEC  1000000l
#define NSEC_PER_USEC 1000l

struct lm3s_clockevent {
	struct clock_event_device dev;
	TIMER_Type *hw;
};

#ifdef QEMU
u64 systick_read(struct clocksource *cs);

static u64 ratio;
static u64 set_next;
#endif

static int lm3s_clkevt_set_next_ktime(ktime_t expires,
				struct clock_event_device *dev)
{
	TIMER_Type *hw = container_of(dev, struct lm3s_clockevent, dev)->hw;
	u32 ticks_per_usec = SystemFrequency / USEC_PER_SEC;

	if (!expires) {
		hw->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;
		hw->ICR |= GPTM_GPTMICR_TATOCINT_Msk;
	}

	/*
	 * We handle granularity of a microsecond, everything smaller is
	 * just noise. SystemFrequency must be in the MHz range.
	 */
	hw->TAILR = (expires / (u64)NSEC_PER_USEC) * ticks_per_usec;
	hw->CTL |= (GPTM_GPTMCTL_TAEN_Enabled << GPTM_GPTMCTL_TAEN_Pos);

#ifdef QEMU
	/* Record the time when timer was started */
	set_next = systick_read(NULL);
#endif

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

static int lm3s_clkevt_set_state_shutdown(struct clock_event_device *dev)
{
	TIMER_Type *hw = container_of(dev, struct lm3s_clockevent, dev)->hw;

	hw->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;
	hw->ICR |= GPTM_GPTMICR_TATOCINT_Msk;

	return 0;
}

static ktime_t lm3s_clkevt_read_elapsed(struct clock_event_device *dev)
{
#ifdef QEMU
	if (!set_next)
		return 0;

	u64 ticks = ((systick_read(NULL) - set_next) * 0x1000000ull) / ratio;

	return clocksource_cyc2ns(ticks , 125, 1);
#else
#warning "Missing implementation"
#endif
}

/* use general purpose timer 0 as clock event device */
static struct lm3s_clockevent lm3s_clockevent = {
	.dev = {
		.set_next_ktime = lm3s_clkevt_set_next_ktime,
		.name = "lm3s-timer0",
		.features = CLOCK_EVT_FEAT_ONESHOT,
		.irq = Timer0A_IRQn,
		.set_state_oneshot = lm3s_clkevt_set_state_oneshot,
		.set_state_shutdown = lm3s_clkevt_set_state_shutdown,
		.read_elapsed = lm3s_clkevt_read_elapsed,
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

#ifdef QEMU
static void emulate_timer_read(void)
{
	pr_info("Compare Timer0 to SysTick tick...");

	//clock_monotonic_suspend(); // disable()

	TIMER0->CTL &= ~GPTM_GPTMCTL_TAEN_Msk;
	TIMER0->CFG = 0;
	TIMER0->TAMR = GPTM_GPTMTAMR_TAMR_OneShot << GPTM_GPTMTAMR_TAMR_Pos;
	TIMER0->TAILR = 0xfffff; /* Pick a random value, SysTick must not wrap-around */

	pr_info("SysTick->LOAD = %08x (max)", 0xffffff);
	pr_info("TIMER0->TAILR = %08x", TIMER0->TAILR);

	SysTick->CTRL = 0;
	SysTick->LOAD = 0xffffff;
	SysTick->VAL = 0;

	/* Start SysTick and Timer0 ``almost'' simultaneously */
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
		| SysTick_CTRL_ENABLE_Msk;
	TIMER0->CTL |= (GPTM_GPTMCTL_TAEN_Enabled << GPTM_GPTMCTL_TAEN_Pos);

	/* Active wait until timer reaches 0 */
	while ((TIMER0->RIS & GPTM_GPTMRIS_TATORIS_Msk) == 0)
		;

	u32 val = SysTick->VAL;
	if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
		pr_err("SysTick must not wrap-around!");
		BUG();
	}
	pr_info("SysTick->VAL  = %08x", val);
	ratio = val;

	clock_monotonic_resume(); // enable()
}
#endif

int lm3s_timer_init(/* struct device_node *node */)
{
#ifdef QEMU
	emulate_timer_read();
#endif

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

	return 0;
}
