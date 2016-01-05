#ifdef DEBUG
#include <stdio.h>
#endif /* DEBUG */

#include "thread.h"
#include "linux/list.h"

static LIST_HEAD(rr_runq);
static struct thread_info thread_idle; //FIXME: implement the idle thread

void sched_rr_add(struct thread_info *thread)
{
	list_add(&thread->ti_list, &rr_runq);
}

void sched_rr_elect(void)
{
	CURRENT_THREAD_INFO(current);
	struct thread_info *next;

#ifdef DEBUG
	printf("current thread is %p\n", current);
#endif /* DEBUG */

	if (list_empty(&rr_runq)) {
		next = &thread_idle;
	} else {
		if (list_is_last(&current->ti_list, &rr_runq)) {
			next = list_first_entry(&rr_runq, struct thread_info, ti_list);
#ifdef DEBUG
			printf("last thread\n");
#endif /* DEBUG */
		} else {
			next = list_next_entry(current, ti_list);
#ifdef DEBUG
			printf("next thread is %p\n", next);
#endif /* DEBUG */
		}
	}
	if (next != current)
		switch_to(next, current);
}
