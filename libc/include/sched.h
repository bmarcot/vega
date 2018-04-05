#pragma once

#include <asm/ptrace.h>

#include "syscall-wrappers.h"

static inline __attribute__((always_inline)) int
clone(unsigned long flags, void *child_stack, struct pt_regs *regs)
{
	return SYS_clone(flags, child_stack, regs);
}

static inline __attribute__((always_inline)) int
sched_yield(void)
{
	return SYS_sched_yield();
}
