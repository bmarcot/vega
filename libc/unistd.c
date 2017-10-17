/* syscall wrappers */

#include <stddef.h>

#include <asm/syscalls.h>
#include "vega/syscalls.h"

#include <sys/types.h>

#include <uapi/kernel/time.h>

#include "syscall-wrappers.h"

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

void _exit(int status)
{
	SYS_exit_group(status);
}
