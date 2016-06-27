#ifndef LIBC_STDLIB_H
#define LIBC_STDLIB_H

#include "linux/types.h"

/* forward declarations */

void kernel_heap_init(void *heap_start, size_t heap_size);
void *malloc(size_t size);
void free(void *ptr);

#endif /* !LIBC_STDLIB_H */
