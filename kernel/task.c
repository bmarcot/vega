/*
 * kernel/task.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <kernel/bitops.h>
#include <kernel/list.h>
#include <kernel/mm/page.h>
#include <kernel/sched.h>
#include <kernel/stddef.h>
#include <kernel/thread.h>

#include <asm/current.h>

static LIST_HEAD(tasks);

static unsigned long tgid_map[THREAD_GROUP_MAX / BITS_PER_LONG];
static unsigned long tid_map[THREAD_MAX / BITS_PER_LONG];

static inline int get_tgid(pid_t *pid)
{
	unsigned long bit = find_first_zero_bit(tgid_map, 32);

	if (bit == 32)
		return -1;
	bitmap_set_bit(tgid_map, bit);
	*pid = bit << 8;

	return 0;
}

static inline void put_tgid(pid_t pid)
{
	bitmap_clear_bit(tgid_map, pid >> 8);
}

static inline int get_tid(pid_t tgid, pid_t *pid)
{
	unsigned long bit = find_first_zero_bit(tid_map, 256);

	if (bit == 256)
		return -1;
	bitmap_set_bit(tid_map, bit);
	*pid = tgid | bit;

	return 0;
}

static inline void put_tid(pid_t pid)
{
	bitmap_clear_bit(tid_map, pid & 0xff);
}

int init_task(struct task_struct *task, int flags)
{
	task->prio = PRI_MIN;
	task->state = TASK_NEW;
	task->stack = &task->thread_info;
	task->filemap = 0;
	task->flags = flags;
	if (flags & CLONE_THREAD) {
		task->exit_signal = -1;
		task->parent = current; // current->parent
		task->tgid = current->tgid;
		get_tid(task->tgid, &task->tid);
	} else {
		task->exit_signal = 0;
		task->parent = NULL;
		get_tgid(&task->tgid);
		task->tid = task->tgid;
	}
	for (int i = 0; i < FILE_MAX; i++)
		task->filetable[i] = NULL;
	list_add(&task->list, &tasks);

	return 0;
}

int release_task_pids(struct task_struct *task)
{
	if (task->flags & CLONE_THREAD)
		put_tid(task->tid);
	else
		put_tgid(task->tgid);

	return 0;
}

void put_task_struct(struct task_struct *tsk)
{
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
		if (tsk->tid == pid)
			return tsk;
	}

	return NULL;
}
