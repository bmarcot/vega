/*
 * kernel/fs/vfs.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs/vfs.h>

#include "linux/list.h"

static LIST_HEAD(vfsdefs);

int vfsdef_register(const char *name, const struct vfsops *vfsops)
{
	struct vfsdef *vfsdefp;

	vfsdefp = malloc(sizeof(struct vfsdef));
	if (vfsdefp == NULL)
		return -1;
	vfsdefp->name = name;
	vfsdefp->vfsops = vfsops;
	list_add(&vfsdefp->list, &vfsdefs);

	return 0;
}

int vfsdef_deregister(const char *name)
{
	struct vfsdef *vfsdefp;

	list_for_each_entry(vfsdefp, &vfsdefs, list) {
		if (!strcmp(vfsdefp->name, name)) {
			list_del(&vfsdefp->list);
			free(vfsdefp);
			break;
		}
	}

	return 0;
}

struct vfsdef *vfsdef_find(const char *name)
{
	struct vfsdef *vfsdefp;

	list_for_each_entry(vfsdefp, &vfsdefs, list) {
		if (!strcmp(vfsdefp->name, name))
			return vfsdefp;
	}

	return NULL;
}


