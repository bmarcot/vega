/*
 * include/kernel/fs/vnode.h
 *
 * Copyright (c) 2016 Benoit Marcot
 */

#ifndef KERNEL_FS_PATHNAME_H
#define KERNEL_FS_PATHNAME_H

int lookuppn(struct vnode *dvp, struct vnode **vpp, const char *pathname);

#endif /* !KERNEL_FS_PATHNAME_H */
