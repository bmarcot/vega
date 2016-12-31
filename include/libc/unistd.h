#ifndef UNISTD_H
#define UNISTD_H

#include <sys/stat.h>
#include <sys/types.h>

enum sc_varname {
	PAGESIZE,
	_SC_CLK_TCK
};

long sysconf(int name);
long sysconf_1(int name);

unsigned int msleep(unsigned int msecs);
unsigned sleep(unsigned seconds);

int open(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, void *buf, size_t count);
off_t seek(int fd, off_t offset, int whence);
int stat(const char *pathname, struct stat *buf);
int mount(const char *source, const char *target, const char *filesystemtype,
	unsigned long mountflags, const void *data);

typedef void DIR;
struct dirent;

DIR *opendir(const char *name);
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int closedir(DIR *dirp);

#endif /* !UNISTD_H */
