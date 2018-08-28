/*
 * kernel/fs/path.c
 *
 * Copyright (c) 2016-2018 Benoit Marcot
 */

#include <kernel/fs/path.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/string.h>

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

void path_release(struct list_head *head)
{
	struct pathcomp *comp, *n;

	list_for_each_entry_safe(comp, n, head, list) {
		list_del(&comp->list);
		kfree(comp);
	}
}
