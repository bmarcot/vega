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

int init_task(struct task_struct *tsk, int flags)
{
	pid_t pid;

	pid = get_pid();
	if (pid == -1)
		return -1;
	tsk->prio = PRI_MIN;
	tsk->state = TASK_NEW;
	tsk->stack = &tsk->thread_info;
	tsk->filemap = 0;
	tsk->flags = flags;
	if (flags & CLONE_THREAD) {
		tsk->group_leader = current->group_leader;
		tsk->exit_signal = -1;
		tsk->parent = current->parent;
		tsk->pid = pid;
		tsk->tgid = current->group_leader->tgid;
	} else {
		tsk->group_leader = tsk; /* self */
		tsk->exit_signal = SIGCHLD;
		tsk->parent = current;
		tsk->pid = pid;
		tsk->tgid = pid;
	}

	/* files */
	for (int i = 0; i < FILE_MAX; i++)
		tsk->filetable[i] = NULL;
	list_add(&tsk->list, &tasks);

	/* signals */
	if (flags & CLONE_THREAD) {
		tsk->signal = current->group_leader->signal;
		tsk->sighand = current->group_leader->sighand;
	} else {
		tsk->signal = alloc_signal_struct(tsk);
		tsk->sighand = alloc_sighand_struct(tsk);
	}
	init_sigpending(&tsk->signal->pending);

	return 0;
}

int release_task_pids(struct task_struct *task)
{
	put_pid(task->pid);

	return 0;
}

void put_signal_struct(struct task_struct *tsk)
{
	struct sigqueue *sig, *tmp;

	if (thread_group_leader(tsk) || (tsk->signal != tsk->group_leader->signal)) {
		list_for_each_entry_safe(sig, tmp, &tsk->signal->pending.list, list) {
			list_del(&sig->list);
			if (!(sig->flags & SIGQUEUE_PREALLOC))
				free(sig);
		}
		free(tsk->signal);
	}
}

void put_sighand_struct(struct task_struct *tsk)
{
	//FIXME: Implement with a reference counter, sighand can be shared by
	// processes. Use less memory when tasks don't use signal, apart from
	// the default signals: SIGTERM, SIGCHLD...
	if (thread_group_leader(tsk))
		free(tsk->sighand);
}

void put_task_struct(struct task_struct *tsk)
{
	if (thread_group_leader(tsk)) {
		put_signal_struct(tsk);
		put_sighand_struct(tsk);
	}
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
