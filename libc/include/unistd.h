#pragma once

#include <sys/stat.h>
#include <sys/types.h>

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);
pid_t getpid(void);
int stat(const char *pathname, struct stat *buf);
