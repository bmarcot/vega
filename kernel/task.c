/*
 * kernel/task.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <stdlib.h>

#include <kernel/bitops.h>
#include <kernel/list.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/signal.h>
#include <kernel/stddef.h>
#include <kernel/thread.h>

#include <asm/current.h>

static LIST_HEAD(tasks);

static unsigned long pidmap[THREAD_MAX / BITS_PER_LONG];

static inline pid_t get_pid(void)
{
	unsigned long bit = find_first_zero_bit(pidmap, 256);

	if (bit == 256)
		return -1;
	bitmap_set_bit(pidmap, bit);

	return (pid_t)bit;
}

static inline void put_pid(pid_t pid)
{
	bitmap_clear_bit(pidmap, pid);
}

int init_task(struct task_struct *task, int flags)
{
	pid_t pid;

	pid = get_pid();
	if (pid == -1)
		return -1;
	task->prio = PRI_MIN;
	task->state = TASK_NEW;
	task->stack = &task->thread_info;
	task->filemap = 0;
	task->flags = flags;
	if (flags & CLONE_THREAD) {
		task->group_leader = current->group_leader;
		task->exit_signal = -1;
		task->parent = current->parent;
		task->pid = pid;
		task->tgid = current->group_leader->tgid;
	} else {
		task->group_leader = task; /* self */
		task->exit_signal = SIGCHLD;
		task->parent = current;
		task->pid = pid;
		task->tgid = pid;
	}

	/* files */
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;
	list_add(&task->list, &tasks);

	/* signals */
	init_sigpending(&task->pending);
	if (flags & CLONE_THREAD)
		task->sighand = current->group_leader->sighand;
	else
		task->sighand = NULL;

	return 0;
}

int release_task_pids(struct task_struct *task)
{
	put_pid(task->pid);

	return 0;
}

void put_sighand_struct(struct task_struct *tsk)
{
	struct sigqueue *sig, *tmp;

	if (tsk->sighand) {
		list_for_each_entry_safe(sig, tmp, &tsk->pending.list, list) {
			list_del(&sig->list);
			if (!(sig->flags & SIGQUEUE_PREALLOC))
				free(sig);
		}
		free(tsk->sighand);
	}
}

void put_task_struct(struct task_struct *tsk)
{
	if (thread_group_leader(tsk))
		put_sighand_struct(tsk);
	list_del(&tsk->list);
	free_pages((unsigned long)tsk->stack, size_to_page_order(THREAD_SIZE));
}

void release_task(struct task_struct *tsk)
{
	release_task_pids(tsk);
}

struct task_struct *get_task_by_pid(pid_t pid)
{
	struct task_struct *tsk;

	list_for_each_entry(tsk, &tasks, list) {
		if (tsk->pid == pid)
			return tsk;
	}

	return NULL;
}
