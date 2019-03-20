/* syscall wrappers */

#include <uapi/kernel/stat.h>
#include <asm/posix_types.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

#include <fcntl.h>    /* For O_* constants */
#include <mqueue.h>
#include <stdarg.h>
#include <stddef.h>

mqd_t mq_open(const char *name, int oflag, ...)
{
	mode_t mode = 0;
	struct mq_attr *attr = NULL;

	if (oflag & O_CREAT) {
		va_list ap;
		va_start(ap, oflag);
		mode = va_arg(ap, int);
		attr = va_arg(ap, void *);
		va_end(ap);
	}

	return syscall(4, name, oflag, mode, attr, SYS_MQ_OPEN);
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
