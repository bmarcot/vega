/*
 * kernel/fs/pathmanip.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <string.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>

int path_head(char *buf, const char *pathname)
{
	int i, i0 = 0;

	if (pathname[0] == '\0')
		return -1;
	if (pathname[0] == '/')
		i0++;
	for (i = i0; i < NAME_MAX && pathname[i] != '/' && pathname[i] != '\0'; i++)
		;
	strncpy(buf, &pathname[i0], i - i0);
	buf[i - i0] = '\0';

	return i;
}
