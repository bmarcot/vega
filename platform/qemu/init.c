/* Qemu system init */

#include "kernel.h"
#include "platform.h"

#define SYSTICK_FREQ_IN_HZ 1000
#define SYSTICK_PERIOD_IN_MSECS (SYSTICK_FREQ_IN_HZ / 1000)

struct timer_operations;

void config_timer_operations(struct timer_operations *tops);

extern struct timer_operations systick_tops;

void __platform_init(void)
{
	config_timer_operations(&systick_tops);

	/* SysTick running at 1kHz */
	printk("Processor speed: %3d MHz\n", CPU_FREQ_IN_HZ / 1000000);
	printk("Timer precision: %3d msec\n", SYSTICK_PERIOD_IN_MSECS);
	SysTick_Config(CPU_FREQ_IN_HZ / SYSTICK_FREQ_IN_HZ);
}
