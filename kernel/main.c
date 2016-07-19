#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>

#include <kernel/mm.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

#include "uart.h"
#include "systick.h"
#include "kernel.h"
#include "version.h"
#include "platform.h"
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
#include "cmsis/arm/ARMCM0.h"
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
#include "cmsis/arm/ARMCM4.h"
#endif

extern char __early_stack_start__;
extern char __early_stack_end__;
extern char __heap_start__;
extern char __heap_size__;

void *cpu_idle(void *);

void __weak *main(__unused void *arg)
{
	return 0;
}

struct thread_info *thread_idle;

/* Cortex-M3 & Cortex-M4 system initialization */
#if __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
static void v7m_init(void)
{
	/* enable UsageFault, BusFault, MemManage faults */
	SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk
		| SCB_SHCSR_MEMFAULTENA_Msk);

	/* follow the architectural requirements */
	__DSB();
}
#endif

struct thread_info *start_kernel(void)
{
	struct thread_info *thread_main;

#if __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	v7m_init();
#endif
	uart_init();

	/* initialize the kernel's malloc */
	kernel_heap_init(&__heap_start__, (size_t) &__heap_size__);

	printk("Version:    %s\n", VER_SLUG);
	printk("Created:    %s  %s UTC\n", __DATE__, __TIME__);

	/* initialize the physical memory allocator */
	page_init();

	/* select a scheduling policy */
	sched_select(SCHED_CLASS_O1);

	/* idle_thread is not added to the runqueue */
	thread_idle = thread_create(cpu_idle, NULL, THREAD_PRIV_SUPERVISOR, 1024);
	if (thread_idle == NULL) {
		printk("[!] Could not create system idle thread.\n");
		return NULL;
	}
	printk("Created idle_thread at %p\n", thread_idle);

	/* The main_thread is the user entry point to the system, and is not added to
	 * the runqueue because it is implicitly "elected".  */
	thread_main = thread_create(main, NULL, THREAD_PRIV_USER, 1024);
	if (thread_main == NULL) {
		printk("[!] Could not create user main thread.\n");
		return NULL;
	}
	printk("Created main_thread at %p with priority=%d\n", thread_main,
		thread_main->ti_priority);

	/* SysTick at 1kHz */
	printk("Processor speed: %3d MHz\n", CPU_FREQ_IN_HZ / 1000000);
	printk("Timer precision: %3d msec\n", SYSTICK_PERIOD_IN_MSECS);
	SysTick_Config(CPU_FREQ_IN_HZ / SYSTICK_FREQ_IN_HZ);

	/* Reclaim the early stack physical memory. In the current context, no
	 * page allocation after this point are allowed.  */
	printk("Reclaim early stack's physical memory (%d Bytes).\n",
		&__early_stack_start__ - &__early_stack_end__);
	page_free(&__early_stack_end__);

	printk("Kernel bootstrap done.\n\n");

	return thread_main;
}

void *cpu_idle(__unused void *arg)
{
	printk("-- in cpu idle --\n");
	for (;;)
		;
}

void system_reset(__unused int reason)
{
	NVIC_SystemReset();
}