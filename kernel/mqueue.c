/*
 * kernel/mqueue.c
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#include <kernel/errno-base.h>
#include <kernel/list.h>
#include <kernel/mqueue.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/syscalls.h>
#include <kernel/types.h>
#include <kernel/wait.h>

#include <uapi/kernel/fcntl.h>

#include <errno.h> //FIXME

static LIST_HEAD(mq_head);

static struct mqdes *find_mq_by_name(const char *name)
{
	struct mqdes *mq;

	list_for_each_entry(mq, &mq_head, list) {
		if (!strncmp(name, mq->name, 16))
			return mq;
	}

	return NULL;
}

static void init_mq_attr(struct mq_attr *attr, const struct mq_attr *other,
	int oflag)
{
	if (other) {
		memcpy(attr, other, sizeof(*attr));
	} else {
		attr->mq_maxmsg = 10;
		attr->mq_msgsize = 256;
		attr->mq_curmsgs = 0;
	}
	attr->mq_flags = oflag & O_NONBLOCK; /* 0 or O_NONBLOCK */
}

SYSCALL_DEFINE(mq_open,
	const char	*name,
	int		oflag,
	mode_t		mode,
	struct mq_attr	*attr)
{
	struct mqdes *mqdes;

	mqdes = find_mq_by_name(name);

	if (mqdes) {
		if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
			errno = EEXIST;
			return -1;
		}
	} else {
		if (!mqdes && (oflag & O_CREAT)) {
			mqdes = kmalloc(sizeof(struct mqdes));
			strncpy(mqdes->name, name, 16);
			init_mq_attr(&mqdes->attr, attr, oflag);
			list_add(&mqdes->list, &mq_head);
			INIT_LIST_HEAD(&mqdes->msg_head);
			INIT_LIST_HEAD(&mqdes->wq_head);
		} else {
			return -1;
		}
	}

	return (int)mqdes;
}

SYSCALL_DEFINE(mq_send,
	struct mqdes	*mqdes,
	const char	*msg_ptr,
	size_t		msg_len,
	unsigned int	msg_prio)
{
	struct mqmsg *new;

	/* Test for empty message queue */
	if (mqdes->attr.mq_curmsgs == mqdes->attr.mq_maxmsg) {
		if (test_mq_attr_nonblock(&mqdes->attr)) {
			errno = EAGAIN;
			return -1;
		} else {
			/* Block until another thread pops a message */
		}
	}

	new = kmalloc(sizeof(*new) + msg_len);
	if (!new)
		return -1;
	new->len = msg_len;
	memcpy(new->msg_ptr, msg_ptr, msg_len);
	mqdes->attr.mq_curmsgs++;
	list_add_tail(&new->list, &mqdes->msg_head);
	if (!test_mq_attr_nonblock(&mqdes->attr))
		wake_up(&mqdes->wq_head, 1);

	return 0;
}

SYSCALL_DEFINE(mq_receive,
	struct mqdes	*mqdes,
	char		*msg_ptr,
	size_t		msg_len,
	unsigned int	*msg_prio)
{
	struct mqmsg *msg;
	int len;

	if (!test_mq_attr_nonblock(&mqdes->attr)) {
		int retval = wait_event_interruptible(&mqdes->wq_head,
						!list_empty(&mqdes->msg_head));
		if (retval == -ERESTARTSYS) {
			errno = EINTR;
			return -1;
		}
	}

	msg = list_first_entry_or_null(&mqdes->msg_head, struct mqmsg, list);
	if (!msg) {
		/* Woke up on new message, but empty message queue */
		if (!test_mq_attr_nonblock(&mqdes->attr))
			BUG();

		errno = EAGAIN;
		return -1;
	}
	len = msg->len;
	memcpy(msg_ptr, msg->msg_ptr, msg->len);
	list_del(&msg->list);
	kfree(msg);

	return len;
}
