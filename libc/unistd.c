/* syscall wrappers */

#include <stddef.h>
#include <sys/types.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

int nanosleep(const struct timespec *req, struct timespec *rem)
{
	return syscall(2, req, rem, SYS_NANOSLEEP);
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
	return syscall(3, filename, argv, envp, SYS_EXECVE);
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
	syscall(1, status, SYS_EXIT_GROUP);
}

int pause(void)
{
	return syscall(0, SYS_PAUSE);
}
