/*
 * arch/v7m-thread.c
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#include <kernel/sched.h>
#include <kernel/types.h>

#include <arch/thread_info.h>
#include <arch/v7m-helper.h>

#include <pthread.h> //FIXME: because of pthread_exit()

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
	thread->kernel_ctx.sp = (__u32)thread + THREAD_SIZE
		- sizeof(struct preserved_context); // msp
	thread->kernel_ctx.ctx->r4_r12[0] = 0; // r4
	thread->kernel_ctx.ctx->r4_r12[1] = 0; // r5
	thread->kernel_ctx.ctx->r4_r12[2] = 0; // r6
	thread->kernel_ctx.ctx->r4_r12[3] = 0; // r7
	thread->kernel_ctx.ctx->r4_r12[4] = 0; // r8
	thread->kernel_ctx.ctx->r4_r12[5] = 0; // r9
	thread->kernel_ctx.ctx->r4_r12[6] = 0; // r10
	thread->kernel_ctx.ctx->r4_r12[7] = 0; // r11
	thread->kernel_ctx.ctx->r4_r12[8] = 0; // r12
	thread->kernel_ctx.ctx->lr = V7M_EXC_RETURN_THREAD_PROCESS;

	thread->thread_ctx.sp = (__u32)stack_start
		- sizeof(struct cpu_saved_context); // psp
	thread->thread_ctx.ctx->r0_r3__r12[0] = (__u32)arg; // r0
	thread->thread_ctx.ctx->r0_r3__r12[1] = 0; // r1
	thread->thread_ctx.ctx->r0_r3__r12[2] = 0; // r2
	thread->thread_ctx.ctx->r0_r3__r12[3] = 0; // r3
	thread->thread_ctx.ctx->r0_r3__r12[4] = 0; // r12
	thread->thread_ctx.ctx->lr = (__u32)pthread_exit;
	//FIXME: Should libc be dynamically loaded?
	//FIXME: Alternatively, let the user manage the thread's last return.
	// i.e. the last return in thread should be explicitely a call exit()
	// or pthread_exit(); not just 'return 0'. If doing so, then take the
	// exception (and core is dumped).
	thread->thread_ctx.ctx->ret_addr =
		(__u32)v7m_clear_thumb_bit(start_routine);
	thread->thread_ctx.ctx->xpsr = xPSR_T_Msk;

	/* if (task->flags == KERNEL_THREAD) // TASK_OPT_KERNEL, OPT_KERNEL */
	/* 	thread->priv = THREAD_PRIV_SUPERVISOR; */
	/* else */
	thread->priv = V7M_UNPRIVILEGED;

	return 0;
}
