/*
 * arch/thread_info.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _ARCH_THREAD_INFO_H
#define _ARCH_THREAD_INFO_H

#include <kernel/types.h>

struct preserved_context {
	__u32 r4_r12[9]; /* zero-filled */
	__u32 lr;        /* loaded with EXC_RETURN value */
};

struct cpu_saved_context {
	__u32 r0_r3__r12[5]; /* r0 to r3, r12; args or zero-filled */
	__u32 lr;            /* initially loaded with pthread_exit() */
	__u32 ret_addr;      /* thread entry-point function */
	__u32 xpsr;          /* forced to Thumb_Mode */
};

struct task_struct;

struct thread_info {
	union {
		__u32 sp; /* msp special register */
		struct preserved_context *ctx;
	} kernel_ctx;
	union {
		__u32 sp; /* psp special register */
		struct cpu_saved_context *ctx;
	} thread_ctx;
	__u32 priv;
#ifdef CONFIG_THREAD_INFO_IN_TASK
	struct task_struct task[0];
#else
	struct task_struct *task;
#endif
};

#define THREAD_SIZE 512

static inline struct thread_info *current_thread_info(void)
{
	__u32 sp;
	__asm__ __volatile__("mov %0, sp" : "=r" (sp));

	return (struct thread_info *)(sp & ~(THREAD_SIZE - 1));
}

#endif /* !_ARCH_THREAD_INFO_H */
