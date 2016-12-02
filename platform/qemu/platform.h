#ifndef PLATFORM_H
#define PLATFORM_H

#include "cmsis/arm/ARMCM4.h"

#define CPU_FREQ_IN_HZ (12 * 1000 * 1000)    /* 12MHz */

#define CONFIG_HAVE_SEMIHOSTING

void __platform_init(void);
void __platform_halt(void);

#endif /* !PLATFORM_H */
