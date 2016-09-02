#ifndef LINUX_KERNEL_H
#define LINUX_KERNEL_H

#include <stddef.h>

#define container_of(ptr, type, member) ({				\
	    const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	    (type *)( (char *)__mptr - offsetof(type,member) );})

#endif /* !LINUX_KERNEL_H */
