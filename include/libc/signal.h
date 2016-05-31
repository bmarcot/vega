#ifndef LIBC_SIGNAL_H
#define LIBC_SIGNAL_H

#include <stddef.h>

typedef struct {
	void *ss_sp;     /* Base address of stack */
	int ss_flags;    /* Flags */
	size_t ss_size;	 /* Number of bytes in stack */
} stack_t;

#endif /* !LIBC_SIGNAL_H */
