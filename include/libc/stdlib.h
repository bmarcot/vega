#ifndef STDLIB_H
#define STDLIB_H

#include "linux/types.h"

#ifndef NULL
#define NULL  ((void*)0)
#endif

/* forward declarations */

void kernel_heap_init(void *heap_start, size_t heap_size);
void *malloc(size_t size);
void free(void *ptr);

#endif /* !STDLIB_H */
