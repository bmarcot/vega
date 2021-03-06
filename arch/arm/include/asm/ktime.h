/*
 * arch/arm/include/asm/ktime.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _ASM_KTIME_H
#define _ASM_KTIME_H

#include <kernel/types.h>

#define KTIME_MAX ((s64)~(1ll << 63))

typedef s64 ktime_t;

#endif /* !_ASM_KTIME_H */
