/* syscall wrappers */

#include <uapi/kernel/stat.h>
#include <asm/posix_types.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

#include <mqueue.h>

mqd_t mq_open(const char *name, int oflag)
{
	return syscall(2, name, oflag, SYS_MQ_OPEN);
}

int mq_send(mqd_t mqdes, const char *msg_ptr,
	size_t msg_len, unsigned int msg_prio)
{
	return syscall(4, mqdes, msg_ptr, msg_len, msg_prio, SYS_MQ_SEND);
}

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr,
		size_t msg_len, unsigned int *msg_prio)
{
	return syscall(4, mqdes, msg_ptr, msg_len, msg_prio, SYS_MQ_RECEIVE);
}
