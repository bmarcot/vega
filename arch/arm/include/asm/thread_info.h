/*
 * arch/arm/include/asm/thread_info.h
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#include <kernel/log2.h>
#include <kernel/types.h>

#include <asm/ptrace.h>

#define THREAD_SIZE	512

struct cpu_kernel_context {
	u32 r4;  /* zero-filled */
	u32 r5;
	u32 r6;
	u32 r7;
	u32 r8;
	u32 r9;
	u32 r10;
	u32 r11;
	u32 r12;
	u32 lr;  /* loaded with EXC_RETURN value */
};

//XXX: Will die..
#ifndef CONFIG_THREAD_INFO_IN_TASK
struct task_struct;
#endif

struct thread_info {
	union {
		u32 msp;
		struct cpu_kernel_context *ctx;
	} kernel;
	union {
		u32		psp;
		struct pt_regs	*regs;
	} user;
	u32			priv;
	u32			flags;

#ifndef CONFIG_THREAD_INFO_IN_TASK
	struct task_struct	*task;
#endif
};

static inline struct thread_info *current_thread_info(void)
{
	struct thread_info *thread;

	__asm__ __volatile__(
		"mov	%0, sp		\n"
		"bfc	%0, #0, %1        "
		: "=r" (thread)
		: "M" (ilog2(THREAD_SIZE))
		);

	return thread;
}

#define TIF_SIGPENDING		0	/* signal pending */
#define TIF_NEED_RESCHED	1	/* rescheduling necessary */

#define _TIF_SIGPENDING		(1 << TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1 << TIF_NEED_RESCHED)

#endif /* !_ASM_THREAD_INFO_H */
