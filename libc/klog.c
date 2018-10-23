#include <asm/syscalls.h>
#include <libvega/syscalls.h>

int klogctl(int type, char *bufp, int len)
{
	return syscall(3, type, bufp, len, SYS_SYSLOG);
}
