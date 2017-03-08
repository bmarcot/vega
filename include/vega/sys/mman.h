#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/types.h>

#define MAP_FAILED ((void *)-1)

#define MAP_ANONYMOUS     0x01 /* don't use a file */
#define MAP_UNINITIALIZED 0x02 /* anonymous memory can be uninitialized */

void *mmap(void *addr, size_t length, int prot, int flags,
	int fd, off_t offset);

#endif /* !_SYS_MMAN_H */
