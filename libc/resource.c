#include <sys/types.h>
#include <asm/syscalls.h>
#include <libvega/syscalls.h>

int getpriority(int which, id_t who)
{
	return syscall(2, which, who, SYS_GETPRIORITY);
}

int setpriority(int which, id_t who, int prio)
{
	return syscall(3, which, who, prio, SYS_SETPRIORITY);
}
