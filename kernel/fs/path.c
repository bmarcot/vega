/*
 * kernel/fs/path.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/fs.h> // For NAME_MAX definition
#include <kernel/fs/path.h>
#include <kernel/kernel.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/string.h>

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

int path_split(struct list_head *head, char *pathname)
{
	char *component;
	int count = 0;

	/* Pass the initial slash in pathname (absolute path) */
	pathname++;

	do {
		component = strsep(&pathname, "/");
		if (component) {
			struct pathcomp *pc = kmalloc(sizeof(struct pathcomp));
			pc->name = component;
			list_add_tail(&pc->list, head);
			count++;
		}
	} while (component);

	return count;
}
