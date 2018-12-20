/*
 * include/kernel/mqueue.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _KERNEL_MQUEUE_H
#define _KERNEL_MQUEUE_H

#include <kernel/types.h>

#include <uapi/kernel/fcntl.h>
#include <uapi/kernel/mqueue.h>

struct mqdes {
	char			name[16];  //FIXME: This will be replaced by an inode in tmpfs
	struct mq_attr		attr;
	struct list_head	list;
	struct list_head	msg_head;
	struct list_head	wq_head;
};

struct mqmsg {
	size_t			len;
	struct list_head	list;
	char			msg_ptr[0];
};

static inline int test_mq_attr_nonblock(struct mq_attr *attr)
{
	return attr->mq_flags & O_NONBLOCK;
}

#endif /* !_KERNEL_MQUEUE_H */
