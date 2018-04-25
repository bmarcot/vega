/*
 * kernel/task.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <stdlib.h>

#include <kernel/bitops.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/mm/page.h>
#include <kernel/ptrace.h>
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

void reserve_pid(pid_t pid)
{
	bitmap_set_bit(pidmap, pid);
}

int init_task(struct task_struct *tsk, int flags)
{
	pid_t pid;

	pid = get_pid();
	if (pid == -1)
		return -1;
	tsk->prio = PRI_MIN - 1; /* lowest prio is reserved for init process */
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
		tsk->signal = current->signal;
		tsk->sighand = current->sighand;
	} else {
		tsk->signal = alloc_signal_struct(tsk);
		if (flags & CLONE_SIGHAND)
			tsk->sighand = current->sighand;
		else
			tsk->sighand = copy_sighand_struct(current); //XXX: copy-on-write?
	}
	list_add(&tsk->thread_group, &tsk->signal->thread_head);
	sigemptyset(&tsk->blocked);
	init_sigpending(&tsk->pending);

	/* mm */
	struct mm_struct *mm;
	if (flags & (CLONE_THREAD /* | CLONE_VM */)) {
		mm = current->mm;
		mm->refcount++;
	} else {
		mm = alloc_mm_struct();
		if (!mm)
			return -1;
		mm->refcount = 1;
	}
	tsk->mm = mm;

	return 0;
}

int release_task_pids(struct task_struct *task)
{
	put_pid(task->pid);

	return 0;
}

void put_signal_struct(struct task_struct *tsk)
{
	//XXX: group_leader of group_exit_task??
	if (thread_group_leader(tsk))
		kfree(tsk->signal);
}

void put_sighand_struct(struct task_struct *tsk)
{
	//FIXME: Implement with a reference counter, sighand can be shared by
	// processes. Use less memory when tasks don't use signal, apart from
	// the default signals: SIGTERM, SIGCHLD...
	if (thread_group_leader(tsk))
		kfree(tsk->sighand);
}

void put_task_struct(struct task_struct *tsk)
{
	struct sigqueue *q, *n;

	list_for_each_entry_safe(q, n, &tsk->pending.list, list) {
		list_del(&q->list);
		if (!(q->flags & SIGQUEUE_PREALLOC))
			kfree(q);
	}
	put_signal_struct(tsk);
	put_sighand_struct(tsk);
	if (thread_group_leader(tsk) && !tsk->mm->refcount)
		put_mm_struct(tsk->mm);
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

struct list_head *get_all_tasks(void)
{
	return &tasks;
}

void add_task(struct task_struct *tsk)
{
	list_add(&tsk->list, &tasks);
}

void dump_task_context(struct task_struct *tsk)
{
	printk("\n--\n");
	printk("Sched:\n");
	printk("    PID: %d, TGID: %d\n", tsk->pid, tsk->tgid);
	printk("    Group leader PID: %d\n", tsk->group_leader->pid);
	printk("    State: %d\n", tsk->state);
	printk("    Flags: %08x\n", tsk->flags);
	printk("    Priority: %d\n", tsk->prio);

	printk("Signals:\n");
	printk("    Blocked: %08x\n", tsk->blocked);
	printk("    Pending: %08x\n", tsk->pending.signal);

	printk("Memory:\n");
	printk("    Size: %d bytes\n", tsk->mm->size);
	printk("    Max size: %d bytes\n", tsk->mm->max_size);

	struct mm_region *region;
	int i = 0;
	list_for_each_entry(region, &tsk->mm->region_head, list) {
		printk("      [region #%02d] range: %p--%p, length: %d bytes\n", i++,
			region->start, (u32)region->start + region->length - 1,
			region->length);
	}

	printk("CPU:\n");
	dump_arch_context(tsk);
	printk("--\n");
}
