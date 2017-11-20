/*
 * arch/arm/include/asm/switch_to.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef _ASM_SWITCH_TO_H
#define _ASM_SWITCH_TO_H

struct thread_info;

extern struct task_struct *resume(struct thread_info *next,
				struct thread_info *prev, int save_prev);

#define switch_to(prev, next, last)					\
	do {								\
		(last) = resume(task_thread_info(next),			\
				task_thread_info(prev),			\
				prev->state < EXIT_ZOMBIE);		\
	} while (0);

#endif /* !_ASM_SWITCH_TO_H */
