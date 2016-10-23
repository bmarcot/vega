/*
 * kernel/fs/vnode.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs/vnode.h>

int vnode_reinit(struct vnode *vn)
{
	memset(vn, 0, sizeof(struct vnode));
	vn->v_type = VNON;
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

//XXX: is it a helper function?
int vn_insert(struct vnode *vp, struct vnode *vp_head)
{
	list_add(&vp->v_list, &vp_head->v_head);
	vp->v_parent = vp_head;

	return 0;
}
