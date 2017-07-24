#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/types.h>
#include <uapi/kernel/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd,
	off_t offset);
int munmap(void *addr, size_t length);

#endif /* !_SYS_MMAN_H */
