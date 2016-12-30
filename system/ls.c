/*
 * system/ls.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdio.h>
/* #include <string.h> */
/* #include <sys/types.h> */
#include <unistd.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

int ls(char *pathname)
{
	(void)pathname;

	struct dirent dirent;
	struct dirent *result;

	DIR *dir = opendir("/dev");
	do {
		readdir_r(dir, &dirent, &result);
		if (result != NULL)
			printk("% 6d %s\n", dirent.d_ino, dirent.d_name);
	} while (result != NULL);
	closedir(dir);

	return 0;
}
