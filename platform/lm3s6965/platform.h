/*
 * platform/lm3s6965/platform.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef _PLATFORM_LM3S_PLATFORM_H
#define _PLATFORM_LM3S_PLATFORM_H

#include <cmsis/lm3s6965/lm3s_cmsis.h>

#define CPU_FREQ_IN_HZ (12 * 1000 * 1000)    /* 12MHz */

void __platform_init(void);
void __platform_halt(void);

#endif /* !_PLATFORM_LM3S_PLATFORM_H */
