#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/page.h>
#include <kernel/scheduler.h>
#include <kernel/task.h>
#include <kernel/thread.h>

#include "version.h"
#include "platform.h"

extern char __early_stack_start__;
extern char __early_stack_end__;
extern char __text_start__;
extern char __text_end__;
extern char __rodata_start__;
extern char __rodata_end__;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __pgmem_start__;
extern char __pgmem_end__;
extern char __pgmem_size__;
extern char __heap_start__;
extern char __heap_end__;
extern char __heap_size__;

void cpu_do_idle(void);
void *cpu_idle(void *);
void mtdram_init(void);
void __printk_init(void);
int minishell(void *options);

struct task_info top_task;

void __weak *main(__unused void *arg)
{
	minishell(NULL);

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

	/* Configure the System Control Register to ensure 8-byte stack
	   alignment */
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;

	/* follow the architectural requirements */
	__DSB();
}
#endif

void print_linker_sections(void)
{
	printk("Memory map:\n");
	printk("  .text	  = %08x--%08x	%6d Bytes\n", &__text_start__,
		&__text_end__, &__text_end__ - &__text_start__);
	printk("  .rodata = %08x--%08x	%6d Bytes\n", &__rodata_start__,
		&__rodata_end__, &__rodata_end__ - &__rodata_start__);
	printk("  .data	  = %08x--%08x	%6d Bytes\n", &__data_start__,
		&__data_end__, &__data_end__ - &__data_start__);
	printk("  .bss	  = %08x--%08x	%6d Bytes\n", &__bss_start__,
		&__bss_end__, &__bss_end__ - &__bss_start__);
	printk("  .heap	  = %08x--%08x	%6d Bytes\n", &__heap_start__,
		&__heap_end__, &__heap_end__ - &__heap_start__);
	printk("  .pgmem  = %08x--%08x	%6d Bytes\n", &__pgmem_start__,
		&__pgmem_end__, &__pgmem_end__ - &__pgmem_start__);
}

struct thread_info *start_kernel(void)
{
#if __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	v7m_init();
#endif

	__printk_init();

	INIT_LIST_HEAD(&top_task.signal_head);

	/* initialize the kernel's malloc */
	kernel_heap_init(&__heap_start__, (size_t) &__heap_size__);

	printk("Vega Kernel Release 1.0rc1 (%s) Build %s armv7e-m\n"
		"Copyright (c) 2015-2016 Benoit Marcot\n\n", VER_MILESTONE,
		VER_COMMIT);
	printk("Version slug: %s\n", VER_SLUG);
	printk("Created: %s  %s UTC\n", __DATE__, __TIME__);
	print_linker_sections();

	/* initialize the physical memory allocator */
	show_page_bitmap(); // init_pages();

	/* select a scheduling policy */
	sched_select(SCHED_CLASS_O1);

	/* idle_thread is not added to the runqueue */
	thread_idle = thread_create(cpu_idle, NULL, THREAD_PRIV_SUPERVISOR, 1024);
	if (thread_idle == NULL) {
		printk("[!] Could not create system idle thread.\n");
		return NULL;
	}
	printk("Created idle_thread at <%p>\n", thread_idle);

	/* The main_thread is the user's entry-point to the system.  It is not
	 * added to the runqueue because it has been implicitly "elected" when
	 * start_kernel() returns.    */
	struct thread_info *thread_main = thread_create(main, NULL,
							THREAD_PRIV_USER, 1024);
	if (thread_main == NULL) {
		printk("[!] Could not create user main thread.\n");
		return NULL;
	}
	printk("Created main_thread at <%p> with priority=%d\n", thread_main,
		thread_main->ti_priority);

	/* Reclaim the early-stack physical memory.  In the current context, no
	 * page allocation after this point are allowed.    */
	printk("Reclaim early stack's physical memory (%d Bytes, order=%d).\n",
		&__early_stack_start__ - &__early_stack_end__,
		size_to_page_order(2048));
	free_pages((unsigned long)&__early_stack_end__, size_to_page_order(2048));

	tmpfs_init();
	memdev_init();
	mtdram_init(); /* create a test mtdram device */

	/* do the platform-specific inits */
	__platform_init();

	printk("Kernel bootstrap done.\n--\n");

	return thread_main;
}

void *cpu_idle(void *arg)
{
	(void)arg;

	for (;;)
		cpu_do_idle();
}

void system_reset(__unused int reason)
{
	NVIC_SystemReset();
}
