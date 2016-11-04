/*
 * kernel/fs/vnode.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>

#include <kernel/fs/vnode.h>

int vnode_reinit(struct vnode *vn)
{
	vn->v_type = VNON;
	vn->v_count = 0;
	INIT_LIST_HEAD(&vn->v_head);

	return 0;
}

struct vnode *vnode_alloc(void)
{
	struct vnode *vn;

	//FIXME: get a vnode from the vnode-cache

	/* no vnode in the cache, allocate a fresh node */
	vn = malloc(sizeof(struct vnode));
	if (!vn)
		return NULL;
	vnode_reinit(vn);

	return vn;
}

void vnode_free(struct vnode *vn)
{
	//FIXME: insert the vnode into the cache

	/* cache is full, free the memory */
	free(vn);
}

int vnode_attach(struct vnode *vn, struct vnode *parent_vn)
{
	list_add(&vn->v_list, &parent_vn->v_head);
	vn->v_parent = parent_vn;

	return 0;
}
