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

SYSCALL_DEFINE(mq_open,
	const char	*name,
	int		oflag)
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
			mqdes->flags = oflag;
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

	new = kmalloc(sizeof(*new) + msg_len);
	if (!new)
		return -1;
	new->len = msg_len;
	memcpy(new->msg_ptr, msg_ptr, msg_len);
	list_add_tail(&new->list, &mqdes->msg_head);
	if (!(mqdes->flags & O_NONBLOCK))
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

	if (!(mqdes->flags & O_NONBLOCK)) {
		int retval = wait_event_interruptible(&mqdes->wq_head,
						!list_empty(&mqdes->msg_head));
		if (retval == -ERESTARTSYS) {
			errno = EINTR;
			return -1;
		}
	}

	msg = list_first_entry_or_null(&mqdes->msg_head, struct mqmsg, list);
	if (!msg) {
		errno = EAGAIN;
		return -1;
	}
	len = msg->len;
	memcpy(msg_ptr, msg->msg_ptr, msg->len);
	list_del(&msg->list);
	kfree(msg);

	return len;
}
