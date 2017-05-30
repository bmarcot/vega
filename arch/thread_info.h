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

struct thread_struct;

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
	struct thread_struct   *ti_struct;

#ifdef CONFIG_KERNEL_STACK_CHECKING
	__u32                  ti_canary[2];
#endif

	char                   ti_stacktop[0]; /* top of kernel stack */
};

static inline struct thread_info *current_thread_info(void)
{
	struct thread_info *this;

	__asm__ __volatile__("mov %0, sp \n\t"
			"bfc %0, #0, %1"
			: "=r" (this)
			: "M" (9));

	return this;
}

#endif /* !_ARCH_THREAD_INFO_H */
