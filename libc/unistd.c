/* syscall wrappers */

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

pid_t fork(void)
{
	return do_syscall0(SYS_FORK);
}

void SYS_exit(int status)
{
	do_syscall1((void *)status, SYS_EXIT);
}
