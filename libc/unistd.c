/* syscall wrappers */

#include <stddef.h>

#include <asm/syscalls.h>
#include "vega/syscalls.h"

#include <sys/types.h>

#include <uapi/kernel/time.h>

int nanosleep(const struct timespec *req, struct timespec *rem)
{
	return do_syscall2((void *)req, (void *)rem, SYS_NANOSLEEP);
}

int close(int fd)
{
	return do_syscall1((void *)fd, SYS_CLOSE);
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
	return do_syscall3((void *)filename, (void *)argv, (void *)envp,
			SYS_EXECVE);
}

pid_t vfork(void)
{
	return do_syscall0(SYS_VFORK);
}

void SYS_exit(int status)
{
	do_syscall1((void *)status, SYS_EXIT);
}

int sleep(int secs)
{
	const struct timespec ts = {
		.tv_sec = secs,
		.tv_nsec = 0,
	};

	return nanosleep(&ts, NULL);
}

int msleep(int msecs)
{
	const struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = msecs * 1000000,
	};

	return nanosleep(&ts, NULL);
}
