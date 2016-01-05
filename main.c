#include <stdio.h>

#include "uart.h"
#include "systick.h"
#include "page.h"
#include "thread.h"
#include "sched-rr.h"

extern void *vector_base;
extern void set_vtor(void *);

int sys_vect[8];
void sc_null(void);
void cpu_idle(void);

int sc_null_1(void)
{
	printf("in syscall\n");

	return 0;
}

#include "pthread.h"

void *my_func(void *arg0)
{
	for (int i = 0; i < 100; i++) {
		printf("in %s, id=%d\n", arg0, (int) pthread_self());
		/* sc_null(); */
		for (int i = 0; i < 900000; i++)
			;
		pthread_yield();
	}
	pthread_exit((void *) 33);

	return NULL;
}

struct thread_info *thread_idle;

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

	sys_vect[0] = (int) sc_null_1;
	sys_vect[1] = (int) thread_yield;
	sys_vect[2] = (int) thread_self;
	sys_vect[3] = (int) thread_exit;

	struct thread_info *t1, *t2;
	char *s1 = "thread A";
	char *s2 = "thread B";
	if ((t1 = thread_create(my_func, s1, THREAD_PRIV_USER)) == NULL) {
		printf("fatal error in thread new 1\n");
	}
	if ((t2 = thread_create(my_func, s2, THREAD_PRIV_USER)) == NULL) {
		printf("fatal error in thread new 2\n");
	}
	sched_rr_add(t1);
	sched_rr_add(t2);

	thread_idle = thread_create(cpu_idle, NULL, THREAD_PRIV_SUPERVISOR);

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
