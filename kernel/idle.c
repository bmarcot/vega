/*
 * kernel/idle.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <kernel/kernel.h>
#include <kernel/sched.h>

static unsigned int	idle_stack[32];
struct task_struct	*idle_task;

void __do_idle(void);

static __attribute__((noreturn)) int do_idle(void *arg)
{
	for (;;)
		__do_idle();
}

int idle_init(void)
{
	/* idle_task is not added to the runqueue */
	idle_task = clone_task(do_idle, &idle_stack[32], 0, NULL);
	if (!idle_task) {
		pr_err("Could not create the idle task");
		return -1;
	}
	pr_info("Created idle_thread at <%p> with pid=%d", idle_task, idle_task->pid);

	return 0;
}
