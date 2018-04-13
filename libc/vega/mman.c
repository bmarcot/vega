#include <sys/types.h>
#include <asm/syscalls.h>
#include <libvega/syscalls.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	return (void *)syscall(6, addr, length, prot, flags, fd, offset, SYS_MMAP);
}

int munmap(void *addr, size_t length)
{
	return syscall(2, addr, length, SYS_MUNMAP);
}
