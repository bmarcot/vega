#pragma once

#include <sys/stat.h>
#include <sys/types.h>

#include <uapi/kernel/time.h>

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);
int stat(const char *pathname, struct stat *buf);
int sleep(int secs);
int msleep(int msecs);

pid_t getpid(void);
pid_t vfork(void);
