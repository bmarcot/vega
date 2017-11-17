/* syscall wrappers */

#include <stddef.h>
#include <time.h>

#include <asm/syscalls.h>
#include "vega/syscalls.h"

#include <sys/types.h>

#include "syscall-wrappers.h"

int execve(const char *filename, char *const argv[], char *const envp[])
{
	return SYS_execve(filename, argv, envp);
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

int pause(void)
{
	return SYS_pause();
}
