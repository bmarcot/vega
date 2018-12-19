/*
 * include/uapi/kernel/mqueue.h
 *
 * Copyright (c) 2018 Benoit Marcot
 */

#ifndef _UAPI_KERNEL_MQUEUE_H
#define _UAPI_KERNEL_MQUEUE_H

struct mq_attr {
	long	mq_flags;	/* Flags: 0 or O_NONBLOCK */
	long	mq_maxmsg;	/* Max. # of messages on queue */
	long	mq_msgsize;	/* Max. message size (bytes) */
	long	mq_curmsgs;	/* # of messages currently in queue */
};

#endif /* _UAPI_KERNEL_MQUEUE_H */
