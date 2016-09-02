/*
 * kernel/fs/vnode.c
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#include <stdlib.h>
#include <string.h>

#include <kernel/fs/vnode.h>

/* Init or re-init a vnode. */
int vn_reinit(struct vnode *vp)
{
	memset(vp, 0, sizeof(struct vnode));
	vp->v_type = VNON;
	INIT_LIST_HEAD(&vp->v_head);

	return 0;
}

/* Allocate a vnode and initialize all of its structures. */
struct vnode *vn_alloc(void)
{
	struct vnode *vp;

	//FIXME: get a vnode from the vnode-cache

	/* no vnode in the cache, allocate a fresh node */
	vp = malloc(sizeof(struct vnode));
	if (vp == NULL)
		return NULL;
	vn_reinit(vp);

	return vp;
}

/* Free a previously allocated vnode. */
void vn_free(struct vnode *vp)
{
	//FIXME: insert the vnode into the cache

	/* cache is full, free the memory */
	free(vp);
}

//XXX: is it a helper function?
int vn_insert(struct vnode *vp, struct vnode *vp_head)
{
	list_add(&vp->v_list, &vp_head->v_head);
	vp->v_parent = vp_head;

	return 0;
}

