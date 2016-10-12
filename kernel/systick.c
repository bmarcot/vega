#include <stdint.h>

#include <kernel/scheduler.h>

#include "systick.h"
#include <if/uart.h>
#include "linux/types.h"
#include "cmsis/arm/ARMCM4.h"

static volatile u32 overflow = 0;
static unsigned long clocktime_in_msec;

void __systick(unsigned long clocktime_in_msec);

void systick(void)
{
	clocktime_in_msec += SYSTICK_PERIOD_IN_MSECS;
	if (overflow == 0xff)
		overflow = 0;
	overflow++;

	__systick(clocktime_in_msec);
}

unsigned long get_clocktime_in_msec(void)
{
	return clocktime_in_msec;
}
