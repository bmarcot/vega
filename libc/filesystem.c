/* syscall wrappers */

#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/fs.h>
#include <kernel/syscalls.h>
#include "vega/syscalls.h"

int open(const char *pathname, int flags)
{
	return do_syscall2((void *)pathname, (void *)flags, SYS_OPEN);
}

ssize_t read(int fd, void *buf, size_t count)
{
	return (ssize_t)do_syscall3((void *)fd, buf, (void *)count, SYS_READ);
}

ssize_t write(int fd, void *buf, size_t count)
{
	return (ssize_t)do_syscall3((void *)fd, buf, (void *)count, SYS_WRITE);
}

off_t seek(int fd, off_t offset, int whence)
{
	return (off_t)do_syscall3((void *)fd, (void *)offset, (void *)whence,
				SYS_SEEK);
}

int stat(const char *pathname, struct stat *buf)
{
	return do_syscall2((void *)pathname, (void *)buf, SYS_STAT);
}

int mount(const char *source, const char *target, const char *filesystemtype,
	unsigned long mountflags, const void *data)
{
	return do_syscall5((void *)source, (void *)target, (void *)filesystemtype,
			(void *)mountflags, (void *)data, SYS_MOUNT);
}

DIR *opendir(const char *name)
{
	return (DIR *)do_syscall2((void *)name, (void *)O_DIRECTORY, SYS_OPEN);
}
