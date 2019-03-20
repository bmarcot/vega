#pragma once

#include <asm/posix_types.h>
#include <uapi/kernel/mqueue.h>

typedef int mqd_t;

mqd_t mq_open(const char *name, int oflag, ...);
int mq_send(mqd_t mqdes, const char *msg_ptr,
	size_t msg_len, unsigned int msg_prio);
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr,
		size_t msg_len, unsigned int *msg_prio);
