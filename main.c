#include <stdio.h>

#include "uart.h"
#include "systick.h"
#include "page.h"
#include "thread.h"
#include "sched-rr.h"

extern void *vector_base;
extern void set_vtor(void *);

void sc_null(void);
void cpu_idle(void);

int sc_null_1(void)
{
	printf("in syscall\n");

	return 0;
}

#include "pthread.h"

void *after(void *arg)
{
	printf("After action: %s\n", arg);
	pthread_exit(0);

	return NULL;
}

void *my_func(void *arg0)
{
	for (int i = 0; i < 100; i++) {
		printf("in %s, id=%d\n", arg0, (int) pthread_self());
		/* sc_null(); */
		for (int i = 0; i < 900000; i++)
			;
		pthread_yield();
	}
	pthread_t id;
	pthread_create(&id, after, "after?\n");
	printf("created thread after id=%d", (int) id);
	pthread_exit((void *)(33 + (int) pthread_self()));

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

	struct thread_info *t1, *t2;
	pthread_t t3;
	char *s1 = "thread A";
	char *s2 = "thread B";
	char *s3 = "thread C";
	if ((t1 = thread_create(my_func, s1, THREAD_PRIV_USER)) == NULL) {
		printf("fatal error in thread new 1\n");
	}
	if ((t2 = thread_create(my_func, s2, THREAD_PRIV_USER)) == NULL) {
		printf("fatal error in thread new 2\n");
	}
	sched_rr_add(t1);
	sched_rr_add(t2);
	//pthread_create(&t3, my_func, s3); pas de syscall ici car on est en supervisor!

	if ((thread_idle = thread_create(cpu_idle, NULL, THREAD_PRIV_SUPERVISOR)) == NULL) {
		printf("fatal: Could not create the idle thread.\n");
		return NULL;
	}

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
