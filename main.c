#include <stdio.h>

#include "uart.h"
#include "systick.h"
#include "page.h"
#include "thread.h"
#include "sched-rr.h"

extern void *vector_base;
extern void set_vtor(void *);

void *my_func(void *thread_id)
{
	for (;;) {
		printf("in thread %d\n", (int) thread_id);
		for (int i = 0; i < 1900000; i++)
			;
		/* pthread_yield(); */
	}

	return NULL;
}

struct thread_info *start_kernel(void)
{
	uart_enable();

	printf("the k2 microkernel v%d.%d\n", 0, 1);

	/* page_alloc(); */
	/* page_alloc(); */
	/* page_alloc(); */
	/* page_alloc(); */
	test_list();

	set_vtor(&vector_base);

	systick_init(0x227811);
	//systick_enable();

	struct thread_info *t1, *t2;
	if ((t1 = thread_create(my_func, (void *) 1)) == NULL) {
		printf("fatal error in thread new 1\n");
	}
	if ((t2 = thread_create(my_func, (void *) 2)) == NULL) {
		printf("fatal error in thread new 2\n");
	}
	sched_rr_add(t1);
	sched_rr_add(t2);

	/* printf("thread created %p\n", t1); */
	/* printf("thread created %p\n", t2); */

	return t1;
}

void cpu_locked(int errno)
{
	printf("%d: cpu locked\n", errno);
	for (;;)
		;
}

void cpu_idle(void)
{
	printf("-- in cpu idle --\n");
	for (;;)
		;
}
