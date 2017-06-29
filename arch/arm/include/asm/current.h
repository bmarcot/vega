/*
 * arch/arm/include/asm/current.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _ASM_CURRENT_H
#define _ASM_CURRENT_H

#include <asm/thread_info.h>

struct task_struct;

static inline struct task_struct *get_current(void)
{
#ifdef CONFIG_THREAD_INFO_IN_TASK
	return (struct task_struct *)current_thread_info();
#else
	return current_thread_info()->task;
#endif
}

#define current get_current()

#endif /* !_ASM_CURRENT_H */
