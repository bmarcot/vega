/*
 * system/ls.c
 *
 * Copyright (c) 2016-2017 Benoit Marcot
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vega/dirent.h>

#include <kernel/device.h>
#include <kernel/fs.h>
#include <kernel/kernel.h>

int ls(int argc, char *argv[])
{
	DIR *dir;
	struct dirent dirent;
	struct dirent *result;
	char pathname[64];
	struct stat st;

	if (argc == 1)
		dir = opendir("/"); //FIXME: get current directory
	else
		dir = opendir(argv[1]);

	do {
		readdir_r(dir, &dirent, &result);
		if (result != NULL) {
			if (!strcmp(dirent.d_name, ".") || !strcmp(dirent.d_name, "..")) {
				printk("  % 6d         %s\n", dirent.d_ino, dirent.d_name);
				continue;
			}

			strcpy(pathname, argv[1]);
			strcat(pathname, "/");
			strcat(pathname, dirent.d_name);
			stat(pathname, &st);
			if (S_ISCHR(st.st_mode))
				printk("c % 6d % 2d,%4d %s\n", st.st_ino,
					MAJOR(st.st_rdev), MINOR(st.st_rdev),
					dirent.d_name);
			else if (S_ISDIR(st.st_mode))
				printk("d % 6d %s\n", st.st_ino, dirent.d_name);
			else if (S_ISREG(st.st_mode))
				printk("- % 6d %s\n", st.st_ino, dirent.d_name);
			else
				printk("? % 6d         %s\n", st.st_ino, dirent.d_name);
		}
	} while (result != NULL);
	closedir(dir);

	return 0;
}
