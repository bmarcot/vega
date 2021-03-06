/*
 * include/uapi/kernel/ioctl.h
 *
 * Copyright (c) 2019 Benoit Marcot
 */

#ifndef _UAPI_KERNEL_IOCTL_H
#define _UAPI_KERNEL_IOCTL_H

#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	14
#define _IOC_DIRBITS	2

#define _IOC_NRMASK	((1 << _IOC_NRBITS) - 1)
#define _IOC_TYPEMASK	((1 << _IOC_TYPEBITS) - 1)
#define _IOC_SIZEMASK	((1 << _IOC_SIZEBITS) - 1)
#define _IOC_DIRMASK	((1 << _IOC_DIRBITS) - 1)

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT + _IOC_SIZEBITS)

/*
 * Direction bits, _IOC_WRITE means userland is writing and kernel is
 * reading. _IOC_READ means userland is reading and kernel is writing.
 */
#define _IOC_NONE	0U
#define _IOC_WRITE	1U
#define _IOC_READ	2U

#define _IOC(dir, type, nr, size) \
	(((dir)  << _IOC_DIRSHIFT) | \
	 ((type) << _IOC_TYPESHIFT) | \
	 ((nr)   << _IOC_NRSHIFT) | \
	 ((size) << _IOC_SIZESHIFT))

/*
 * Used to create numbers. _IOW means userland is writing and kernel
 * is reading. _IOR means userland is reading and kernel is writing.
 */
#define _IO(type, nr)		  _IOC(_IOC_NONE, (type), (nr), 0)
#define _IOR(type, nr, size)	  _IOC(_IOC_READ, (type), (nr), sizeof(size))
#define _IOW(type, nr, size)	  _IOC(_IOC_WRITE, (type), (nr), sizeof(size))
#define _IOWR(type, nr, size)	  _IOC(_IOC_READ|_IOC_WRITE, (type), (nr), sizeof(size))
#define _IOR_BAD(type, nr, size)  _IOC(_IOC_READ, (type), (nr), sizeof(size))
#define _IOW_BAD(type, nr, size)  _IOC(_IOC_WRITE, (type), (nr), sizeof(size))
#define _IOWR_BAD(type, nr, size) _IOC(_IOC_READ|_IOC_WRITE, (type), (nr), sizeof(size))

/* Used to decode ioctl numbers */
#define _IOC_DIR(nr)	(((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(nr)	(((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(nr)	(((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(nr)	(((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

#endif /* !_UAPI_KERNEL_IOCTL_H */
