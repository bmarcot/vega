/*
 * kernel/wait.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/list.h>
#include <kernel/sched.h>
#include <kernel/wait.h>

int wake_up(struct list_head *wq_head, int nr)
{
	struct wq_entry *wq_entry;
	int wake_count = 0;

	list_for_each_entry(wq_entry, wq_head, list) {
		sched_enqueue(wq_entry->tsk);
		wake_count++;
		if (wake_count == nr)
			break;
	}

	return wake_count;
}
