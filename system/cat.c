/*
 * system/cat.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <fcntl.h>
#include <sys/cdefs.h>
#include <unistd.h>

#include <kernel/kernel.h>

int cat(__unused int argc, char *argv[])
{
	char buf[] = {0, 0};
	int fd;
	int retval = 0;

	for (int i = 1; i < argc; i++) {
		fd = open(argv[i], 0);
		if (fd < 0) {
			printk("cat: %s: No such file or directory\n",
				argv[1]);
			retval = 1;
		}
		while (read(fd, &buf, 1))
			printk("%s", buf);
		close(fd);
	}

	return retval;
}
