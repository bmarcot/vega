/*
 * include/kernel/thread_info.h
 *
 * Copyright (c) 2017 Benoit Marcot
 */

#ifndef _KERNEL_THREAD_INFO_H
#define _KERNEL_THREAD_INFO_H

#include <kernel/bitops.h>

#include <asm/current.h>
#include <asm/thread_info.h>

static inline void set_ti_thread_flag(struct thread_info *ti, int flag)
{
	set_bit(flag, (unsigned long *)&ti->flags);
}

static inline void clear_ti_thread_flag(struct thread_info *ti, int flag)
{
	clear_bit(flag, (unsigned long *)&ti->flags);
}

static inline int test_ti_thread_flag(struct thread_info *ti, int flag)
{
	return test_bit(flag, (unsigned long *)&ti->flags);
}

#define set_thread_flag(flag) \
	set_ti_thread_flag(current_thread_info(), flag)
#define clear_thread_flag(flag) \
	clear_ti_thread_flag(current_thread_info(), flag)
#define test_thread_flag(flag) \
	test_ti_thread_flag(current_thread_info(), flag)

#endif /* !_KERNEL_THREAD_INFO_H */
