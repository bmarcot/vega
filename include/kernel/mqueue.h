/*
 * include/kernel/mqueue.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_MQUEUE_H
#define _KERNEL_MQUEUE_H

#include <kernel/types.h>

struct mqdes {
	char			name[16];
	int			flags;
	struct list_head	list;
	struct list_head	msg_head;
	struct list_head	wq_head;
};

struct mqmsg {
	size_t			len;
	struct list_head	list;
	char			msg_ptr[0];
};

#endif /* !_KERNEL_MQUEUE_H */
