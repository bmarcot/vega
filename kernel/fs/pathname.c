/*
 * kernel/fs/pathname.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <string.h>

#include <kernel/fs/vnode.h>

#include "kernel.h"

/* pathname management, navigation, traversal functions */

int pn_head(const char *pathname, const char **head/* , int *len */)
{
	const char *p = pathname;

	while (*p == '/')
		p++;
	*head = p;
	while (*p && (*p != '/'))
		p++;

	return p - *head;
}

struct vnode *pn_getcomponent(struct vnode *dvp, const char *pathname, const char **remaining)
{
	const char *compname;
	int len;
	struct vnode *vp;

	len = pn_head(pathname, &compname/* , &len */);
	printk("-- compo: %s %s (%d)\n", pathname, compname, len);
	if (!len)
		return NULL;
	list_for_each_entry(vp, &dvp->v_head, v_list) {
		if (!strncmp(vp->v_path, compname, len)) {
			*remaining = compname + len;
			printk("(fs) found a vnode\n");
			return vp;
		}
	}

	return NULL;
}

/* lookuppn(vnode['/'], '/mnt/hda/intro.txt') */
int lookuppn(struct vnode *dvp, struct vnode **vpp, const char *pathname)
{
	struct vnode *vp;
	const char *remaining_path;

	printk("in: lookuppn(pathname=\"%s\")\n", pathname);
	/* search from ROOTVN or in current dir */

	/* if (pathname[0] == '/') */
	/* 	dvp = ROOTVN; */

	for (;;) {
		vp = pn_getcomponent(dvp, pathname, &remaining_path);
		printk("    component=\"%s\", remaining_path=\"%s\"\n",
			vp->v_path, remaining_path);
		if (vp) {
			if (!strlen(remaining_path)) {
				*vpp = vp;
				return 0;
				//return vp;    /* found the file */
			} else {
				printk("remaining_path=%s   %s\n ", remaining_path, vp->v_path);
				dvp = vp;
				pathname = remaining_path;
			}
		} else {
			if (remaining_path[0] == '/')
				remaining_path++;
			VOP_LOOKUP(dvp, vpp, remaining_path);
		}
	}

	return 0;
}

