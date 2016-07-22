#ifndef PLATFORM_H
#define PLATFORM_H

#include "nrf.h"
#include "cmsis/arm/core_cm4.h"

#define CPU_FREQ_IN_HZ  (64 * 1000 * 1000)    /* 64MHz */

void nrf52_timer_init(void);

static inline void __platform_init(void)
{
	nrf52_timer_init();
}

#endif /* !PLATFORM_H */
