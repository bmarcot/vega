#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/types.h>

void *mmap(void *addr, size_t length, int prot, int flags,
	int fd, off_t offset);

#endif /* !_SYS_MMAN_H */
