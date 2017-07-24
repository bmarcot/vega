/*
 * include/uapi/kernel/mman.h
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 */

#ifndef _UAPI_KERNEL_MMAN_H
#define _UAPI_KERNEL_MMAN_H

#define PROT_NONE  0x00    /* page can not be accessed */
#define PROT_READ  0x01    /* page can be read */
#define PROT_WRITE 0x02    /* page can be written */
#define PROT_EXEC  0x04    /* page can be executed */

#define MAP_FAILED ((void *)-1)

#define MAP_ANONYMOUS     0x01 /* don't use a file */
#define MAP_UNINITIALIZED 0x02 /* anonymous memory can be uninitialized */

#endif /* !_UAPI_KERNEL_MMAN_H */
