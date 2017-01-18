/*
 * system/ls.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdio.h>
#include <unistd.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

int ls(int argc, char *argv[])
{
	DIR *dir;
	struct dirent dirent;
	struct dirent *result;

	if (argc == 1)
		dir = opendir("/"); //FIXME: get current directory
	else
		dir = opendir(argv[1]);

	do {
		readdir_r(dir, &dirent, &result);
		if (result != NULL)
			printk("% 6d %s\n", dirent.d_ino, dirent.d_name);
	} while (result != NULL);
	closedir(dir);

	return 0;
}
