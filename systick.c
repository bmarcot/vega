#include <stdint.h>

#include "systick.h"
#include "uart.h"
#include "sched-rr.h"
#include "linux/types.h"
#include "timer.h"
#include "cmsis/arm/ARMCM4.h"

static volatile u32 overflow = 0;
u32 clocktime_in_msecs;

extern struct list_head timers;

void systick(void)
{
	clocktime_in_msecs += SYSTICK_PERIOD_IN_MSECS;
	if (overflow == 0xff)
		overflow = 0;
	overflow++;

	struct timer *pos;
	list_for_each_entry(pos, &timers, list) {
		if (pos->expire_clocktime < clocktime_in_msecs) {
			list_del(&pos->list);
			sched_rr_add(pos->tip);
			sched_rr_elect_reset();
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
