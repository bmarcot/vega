#ifndef PLATFORM_H
#define PLATFORM_H

#include "cmsis/arm/ARMCM4.h"

#define CPU_FREQ_IN_HZ (12 * 1000 * 1000)    /* 12MHz */

/* Qemu platform use the SysTick to keep a global clock time. The clocktime
   is incremented on every tick.  Timers and sleep functions access  this
   * clocktime.  */
#define CONFIG_HAVE_SYSTICK

static inline void __platform_init(void)
{
	while (0)
		;
}

#endif /* !PLATFORM_H */
