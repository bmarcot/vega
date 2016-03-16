#include <stdio.h>

#include "uart.h"
#include "systick.h"
#include "thread.h"
#include "sched-rr.h"
#include "mm.h"

extern void *vector_base;
extern void set_vtor(void *);

void *cpu_idle(void *);

extern void *main(void *);

struct thread_info *thread_idle;

struct thread_info *start_kernel(void)
{
	uart_enable();
	set_vtor(&vector_base);
	systick_init(0x227811);
	/* systick_enable(); */

	/* initialize the physical memory allocator */
	page_init();

	/* thread_main is the user entry point to the system */
	struct thread_info *thread_main;
	if ((thread_main = thread_create(main, NULL, THREAD_PRIV_USER)) == NULL)
		printf("[!] Could not create user main thread.\n");
	sched_rr_add(thread_main);

	/* the idle thread is not pushed in the rr-runqueue */
	if ((thread_idle = thread_create(cpu_idle, NULL, THREAD_PRIV_SUPERVISOR)) == NULL) {
		printf("[!] Could not create system idle thread.\n");
		return NULL;
	}

	return thread_main;
}


void cpu_locked(int errno)
{
	printf("%d: cpu locked\n", errno);
	for (;;)
		;
}

void *cpu_idle(__unused void *arg)
{
	printf("-- in cpu idle --\n");
	for (;;)
		;
}
