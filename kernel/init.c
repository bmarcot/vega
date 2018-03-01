/*
 * kernel/init.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/kernel.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/sched/signal.h>
#include <kernel/signal.h>
#include <kernel/signal_types.h>
#include <kernel/stddef.h>

#define CONFIG_INIT_STACK_SIZE	512

int main();

static char stack[CONFIG_INIT_STACK_SIZE] __attribute__((aligned(8)));
static struct signal_struct signal;
static struct sighand_struct sighand;

struct task_struct *alloc_init_task(void)
{
	struct task_struct *init;

	init = alloc_pages(size_to_page_order(THREAD_SIZE));
	BUG_ON(!init);

	/* pids */
	reserve_pid(1);
	init->pid = 1;
	init->tgid = 1;

	init->prio = PRI_MIN;
	init->state = TASK_NEW;
	init->stack = &init->thread_info;
	init->flags = 0;

	init->group_leader = init;
	init->exit_signal = -1;
	init->parent = NULL;

	/* signals */
	init->signal = &signal;
	init->sighand = &sighand;
	init_sigpending(&init->pending);

	__thread_setup(init, main, NULL, stack,	CONFIG_INIT_STACK_SIZE);
	add_task(init);
	sched_enqueue(init);

	return init;
}
