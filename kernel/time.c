/*
 * kernel/time.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/types.h>

#include <kernel/bitops.h>
#include <kernel/time.h>
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

static unsigned long timerid_bitmap;

int reserve_timer_id(timer_t *timerid)
{
	unsigned long bit;

	if (timerid == NULL)
		return -1;
	bit = find_first_zero_bit(&timerid_bitmap, BITS_PER_LONG);
	if (bit == BITS_PER_LONG)
		return -1;
	bitmap_set_bit(&timerid_bitmap, bit);
	*timerid = bit;

	return 0;
}
