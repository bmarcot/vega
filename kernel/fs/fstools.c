/*
 * kernel/fs/fstools.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <sys/stat.h>
#include <sys/types.h>
#include "kernel.h"

//FIXME: replace with #include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
int close(int fd);

//FIXME: replace with #include <fcntl.h>
int open(const char *pathname, int flags);

int cat(const char *pathname)
{
	int fd = open(pathname, 0);
	if (fd < 0) {
		printk("error: failed to open %s\n", pathname);
		return -1;
	}

	int count;
	char buf[32];
	for (;;) {
		count = read(fd, buf, 31);
		buf[count] = '\0';
		printk("%s", buf);
		if (count < 31)
			break;
	}

	if (close(fd)) {
		printk("error: failed to close %s\n", pathname);
		return -1;
	}

	return 0;
}
