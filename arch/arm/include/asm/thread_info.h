/*
 * arch/arm/include/asm/thread_info.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#include <kernel/types.h>

struct preserved_context {
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

struct cpu_saved_context {
	u32 r0;        /* args or zero-filled */
	u32 r1;
	u32 r2;
	u32 r3;
	u32 r12;
	u32 lr;        /* initially loaded with pthread_exit() */
	u32 ret_addr;  /* thread entry-point function */
	u32 xpsr;      /* forced to Thumb_Mode */
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
#ifndef CONFIG_THREAD_INFO_IN_TASK
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

#endif /* !_ASM_THREAD_INFO_H */
