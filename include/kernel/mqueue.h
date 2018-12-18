/*
 * include/kernel/mqueue.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_MQUEUE_H
#define _KERNEL_MQUEUE_H

#include <kernel/types.h>

struct mq_attr {
	long	mq_flags;	/* Flags: 0 or O_NONBLOCK */
	long	mq_maxmsg;	/* Max. # of messages on queue */
	long	mq_msgsize;	/* Max. message size (bytes) */
	long	mq_curmsgs;	/* # of messages currently in queue */
};

struct mqdes {
	char			name[16]; //FIXME: This will be replaced by an inode in tmpfs
	struct mq_attr		attr;
	int			flags; //XXX: Will die
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
