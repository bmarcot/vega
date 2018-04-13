/* Newlib stubs */

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/kernel.h>

#include <asm/syscalls.h>
#include <libvega/syscalls.h>

#define HANGS_ON() \
	({ printk("error: Newlib needs %s", __func__); for (;;); })

int _isatty(__unused int file)
{
	HANGS_ON();

	return 1;
}

int _fstat()
{
	HANGS_ON();

	return -1;
}

void *_sbrk(__unused int incr)
{
	HANGS_ON();

	return NULL;
}

clock_t _times()
{
	HANGS_ON();

	return -1;
}

void _fini(void)
{
	HANGS_ON();
}

int _getpid(void)
{
	return syscall(0, SYS_GETPID);
}

pid_t vfork(void)
{
	return (pid_t)syscall(0, SYS_VFORK);
}

pid_t waitpid(pid_t pid, int *status, int options)
{
	return (pid_t)syscall(3, pid, status, options, SYS_WAITPID);
}

pid_t gettid(void)
{
	return (pid_t)syscall(0, SYS_GETTID);
}

pid_t getppid(void)
{
	return (pid_t)syscall(0, SYS_GETPPID);
}

int tgkill(int tgid, int tid, int sig)
{
	return syscall(3, tgid, tid, sig, SYS_TGKILL);
}
