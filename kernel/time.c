#include <sys/types.h>
#include <kernel/timer.h> // struct timer
#include "linux/list.h"

struct timer *find_timer_by_id(timer_t timer_id, struct list_head *timer_list)
{
	struct timer *pos;

	list_for_each_entry(pos, timer_list, list) {
		if (pos->timer_id == timer_id)
			return pos;
	}

	return NULL;
}
