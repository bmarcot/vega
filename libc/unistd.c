/* syscall wrappers */

#include <kernel/syscalls.h>
#include "vega/syscalls.h"

long sysconf(int name)
{
	return do_syscall1((void *)name, SYS_SYSCONF);
}

unsigned int msleep(unsigned int msecs)
{
	return do_syscall1((void *)msecs, SYS_MSLEEP);
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
