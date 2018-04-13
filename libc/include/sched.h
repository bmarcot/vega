#pragma once

#include <asm/ptrace.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

static inline __attribute__((always_inline)) int
clone(unsigned long flags, void *child_stack, struct pt_regs *regs)
{
	return syscall(3, flags, child_stack, regs, SYS_CLONE);
}

static inline __attribute__((always_inline)) int
sched_yield(void)
{
	return syscall(0, SYS_SCHED_YIELD);
}
