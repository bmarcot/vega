#include <stdint.h>

#include <kernel/scheduler.h>
#include <kernel/timer.h>

#include "systick.h"
#include "uart.h"
#include "linux/types.h"
#include "cmsis/arm/ARMCM4.h"

static volatile u32 overflow = 0;
u32 clocktime_in_msecs;

extern struct list_head timers;
extern struct thread_info *thread_idle;

void systick(void)
{
	clocktime_in_msecs += SYSTICK_PERIOD_IN_MSECS;
	if (overflow == 0xff)
		overflow = 0;
	overflow++;

	struct timer *pos;
	list_for_each_entry(pos, &timers, list) {
		if (pos->expire_clocktime < clocktime_in_msecs) {
			/* printk("timer expired!\n"); */
			list_del(&pos->list);
			sched_add(pos->owner);
			CURRENT_THREAD_INFO(current);
			if (current != thread_idle)
				sched_add(current);
			sched_elect(SCHED_OPT_NONE);
			return;
		}
	}
}

u32 gettick(void)
{
	u32 of = overflow;
	u32 val = syst->syst_cvr;
	u32 new_of = overflow;

	if (of != new_of) {
		val = syst->syst_cvr;
		of = new_of;
		uart_putstring("SysTick overflowed.\n");
	}

	return (of << 24) | (SYST_RELOAD_VAL - val);
}

//TODO: calib_pseudo_10ms_wait()
