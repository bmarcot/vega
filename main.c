#include <stdio.h>

#include "uart.h"
#include "systick.h"
#include "thread.h"
#include "sched-rr.h"
#include "mm.h"
#include "kernel.h"
#include "version.h"
#include "platform.h"
#include "cmsis/arm/ARMCM4.h"

extern char vector_base;
extern void set_vtor(void *);

void *cpu_idle(void *);

void __weak *main(__unused void *arg)
{
	return 0;
}

struct thread_info *thread_idle;

/* Cortex-M4 system initialization */
static void cm4_init(void)
{
	/* enable UsageFault, BusFault, MemManage faults */
	SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk
		| SCB_SHCSR_MEMFAULTENA_Msk);

	/* use the second-stage exception vector */
	SCB->VTOR = (int) &vector_base;

	/* follow the architectural requirements */
	__DSB();
}

struct thread_info *start_kernel(void)
{
	cm4_init();
	uart_init();

	printk("Version:    %s\n", VER_SLUG);
	printk("Created:    %s  %s UTC\n", __DATE__, __TIME__);

	/* initialize the physical memory allocator */
	page_init();

	/* the idle thread is not pushed in the rr-runqueue */
	if ((thread_idle = thread_create(cpu_idle, NULL, THREAD_PRIV_SUPERVISOR)) == NULL) {
		printk("[!] Could not create system idle thread.\n");
		return NULL;
	}

	/* thread_main is the user entry point to the system */
	struct thread_info *thread_main;
	if ((thread_main = thread_create(main, NULL, THREAD_PRIV_USER)) == NULL)
		printk("[!] Could not create user main thread.\n");
	sched_rr_add(thread_main);

	/* systick at 1kHz */
	printk("Processor speed: %dMHz\n", CPU_FREQ_HZ / 1000000);
	printk("Timer precision: %dms\n", SYSTICK_PERIOD_IN_MSECS);
	systick_init(CPU_FREQ_HZ / SYSTICK_FREQ);
	systick_enable();

	return thread_main;
}


void cpu_locked(int errno)
{
	printk("%d: cpu locked\n", errno);
	for (;;)
		;
}

void *cpu_idle(__unused void *arg)
{
	printk("-- in cpu idle --\n");
	for (;;)
		;
}
