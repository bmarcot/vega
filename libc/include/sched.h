#pragma once

#include "syscall-wrappers.h"

static inline __attribute__((always_inline)) int
clone(int (*fn)(void *), void *child_stack, int flags, void *arg)
{
	return SYS_clone(fn, child_stack, flags, arg);
}

static inline __attribute__((always_inline)) int
sched_yield(void)
{
	return SYS_sched_yield();
}
