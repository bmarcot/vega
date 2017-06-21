/*
 * include/uapi/kernel/futex.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _UAPI_KERNEL_FUTEX_H
#define _UAPI_KERNEL_FUTEX_H

enum futex_op {
	FUTEX_WAIT,
	FUTEX_WAKE,
};

#endif /* !_UAPI_KERNEL_FUTEX_H */
