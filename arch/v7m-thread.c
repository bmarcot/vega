/*
 * arch/v7m-thread.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/sched.h>
#include <kernel/types.h>

#include <asm/thread_info.h>
#include <asm/v7m-helper.h>

#include <pthread.h> //FIXME: because of pthread_exit()

#include "platform.h"

#include <kernel/kernel.h>

void v7m_task_start_trampoline(void);

// setup_init_task (or preallocated struct in .rodata/.data) then only do
// copy_thread()...
int arch_thread_setup(struct task_struct *task, void *start_routine,
		void *arg, void *stack_start)
{
	struct thread_info *thread = task_thread_info(task);

	/* First task scheduling:
	 *   Non-scratch registers for the user context.
	 *
	 * Subsequent task scheduling:
	 *   Non-scratch registers for the kernel context (non-scratch
	 *   registers for the user context are implicitely saved/restored
	 *   in kernel-stack by function prologues/epilogues.
	 */
	thread->kernel.msp = (__u32)thread + THREAD_SIZE
		- sizeof(struct cpu_kernel_context) - 8; // msp
	thread->kernel.ctx->r4 = 0;
	thread->kernel.ctx->r5 = 0;
	thread->kernel.ctx->r6 = 0;
	thread->kernel.ctx->r7 = 0;
	thread->kernel.ctx->r8 = 0;
	thread->kernel.ctx->r9 = 0;
	thread->kernel.ctx->r10 = 0;
	thread->kernel.ctx->r11 = 0;
	thread->kernel.ctx->r12 = 0;
	thread->kernel.ctx->lr = (u32)v7m_task_start_trampoline;

	thread->user.psp = (__u32)stack_start
		- sizeof(struct cpu_user_context); // psp
	thread->user.ctx->r0 = (__u32)arg;
	thread->user.ctx->r1 = 0;
	thread->user.ctx->r2 = 0;
	thread->user.ctx->r3 = 0;
	thread->user.ctx->r12 = 0;
	thread->user.ctx->lr = (__u32)pthread_exit;
	//FIXME: Should libc be dynamically loaded?
	//FIXME: Alternatively, let the user manage the thread's last return.
	// i.e. the last return in thread should be explicitely a call exit()
	// or pthread_exit(); not just 'return 0'. If doing so, then take the
	// exception (and core is dumped).
	thread->user.ctx->ret_addr =
		(__u32)v7m_clear_thumb_bit(start_routine);
	thread->user.ctx->xpsr = xPSR_T_Msk;

	thread->user.ctx->r4 = 0;
	thread->user.ctx->r5 = 0;
	thread->user.ctx->r6 = 0;
	thread->user.ctx->r7 = 0;
	thread->user.ctx->r8 = 0;
	thread->user.ctx->r9 = 0;
	thread->user.ctx->r10 = 0;
	thread->user.ctx->r11 = 0;

	/* if (task->flags == KERNEL_THREAD) // TASK_OPT_KERNEL, OPT_KERNEL */
	/* 	thread->priv = THREAD_PRIV_SUPERVISOR; */
	/* else */
	thread->priv = V7M_UNPRIVILEGED;

	return 0;
}
