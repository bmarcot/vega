/*
 * arch/arm/kernel/v7m-thread.c
 *
 * Copyright (c) 2017-2018 Benoit Marcot
 */

#include <kernel/sched.h>
#include <kernel/string.h>
#include <kernel/types.h>

#include <asm/ptrace.h>
#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

#include "platform.h"

void v7m_task_start_trampoline(void);

/* child_stack usually points to the topmost address of the memory space set up
 * for the child stack */
void arch_thread_setup(struct task_struct *tsk, unsigned long flags,
		void *child_stack, struct pt_regs *regs)
{
	struct thread_info *thrd = task_thread_info(tsk);

	/* setup kernel context */
	thrd->kernel.msp = (u32)thrd + THREAD_SIZE
		- sizeof(struct cpu_kernel_context) - 8;
	thrd->kernel.ctx->lr = (u32)v7m_task_start_trampoline;

	/* setup user context */
	thrd->user.psp = (u32)child_stack - sizeof(*regs);
	memcpy(thrd->user.regs, regs, sizeof(*regs));
	thrd->user.regs->pc = (u32)v7m_clear_thumb_bit((void *)regs->pc);
	thrd->user.regs->status = xPSR_T_Msk;

	/* if (task->flags == KERNEL_THREAD) // TASK_OPT_KERNEL, OPT_KERNEL */
	/* 	thread->priv = THREAD_PRIV_SUPERVISOR; */
	/* else */
	thrd->priv = V7M_UNPRIVILEGED;
	thrd->flags = 0;
}
