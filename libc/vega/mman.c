#include "syscall-wrappers.h"

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	return SYS_mmap(addr, length, prot, flags, fd, offset);
}

int munmap(void *addr, size_t length)
{
	return SYS_munmap(addr, length);
}
