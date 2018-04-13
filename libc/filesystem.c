/* syscall wrappers */

#include <uapi/kernel/stat.h>
#include <asm/posix_types.h>

#include <kernel/fs.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

int _open(const char *pathname, int flags)
{
	return syscall(2, pathname, flags, SYS_OPEN);
}

int _close(int fd)
{
	return syscall(1, fd, SYS_CLOSE);
}

ssize_t _read(int fd, void *buf, size_t count)
{
	return (ssize_t)syscall(3, fd, buf, count, SYS_READ);
}

ssize_t _write(int fd, void *buf, size_t count)
{
	return (ssize_t)syscall(3, fd, buf, count, SYS_WRITE);
}

off_t _lseek(int fd, off_t offset, int whence)
{
	return (off_t)syscall(3, fd, offset, whence, SYS_LSEEK);
}

int _stat(const char *pathname, struct stat *buf)
{
	return syscall(2, pathname, buf, SYS_STAT);
}

int mount(const char *source, const char *target, const char *filesystemtype,
	unsigned long mountflags, const void *data)
{
	return syscall(5, source, target, filesystemtype, mountflags, data, SYS_MOUNT);
}

DIR *opendir(const char *name)
{
	return (DIR *)syscall(2, name, O_DIRECTORY, SYS_OPEN);
}

int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
	return syscall(3, dirp, entry, result, SYS_READDIR_R);
}

int closedir(DIR *dirp)
{
	return syscall(1, dirp, SYS_CLOSE);
}
