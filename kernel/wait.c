/*
 * kernel/wait.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/list.h>
#include <kernel/sched.h>
#include <kernel/wait.h>

int default_wake_function(struct wait_queue_entry *wq_entry)
{
	set_task_state(wq_entry->private, TASK_RUNNING);

	return 0;
}

int autoremove_wake_function(struct wait_queue_entry *wq_entry)
{
	int ret;

	ret = default_wake_function(wq_entry);
	if (ret)
		list_del_init(&wq_entry->list);

	return ret;
}

void init_wait_queue_entry(struct wait_queue_entry *wq_entry)
{
	wq_entry->private = current;
	wq_entry->func = autoremove_wake_function;
	INIT_LIST_HEAD(&wq_entry->list);
}

int prepare_to_wait_event(struct list_head *wq_head,
			struct wait_queue_entry *wq_entry, int state)
{
	set_current_state(TASK_UNINTERRUPTIBLE);
	list_add(&wq_entry->list, wq_head);

	return 0;
}

void finish_wait(struct wait_queue_entry *wq_entry)
{
	set_current_state(TASK_RUNNING);
	list_del_init(&wq_entry->list);
}

int wake_up(struct list_head *wq_head, int nr)
{
	struct wait_queue_entry *curr, *n;
	int ret, wake_count = 0;

	list_for_each_entry_safe(curr, n, wq_head, list) {
		ret = curr->func(curr);
		if (ret < 0)
			break;
		if (++wake_count == nr)
			break;
	}

	return wake_count;
}
