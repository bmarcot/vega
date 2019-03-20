#include <asm/syscalls.h>
#include <libvega/syscalls.h>

#include <stdarg.h>
#include <stddef.h>

#include <uapi/kernel/ioctl.h>

int ioctl(int fd, unsigned long request, ...)
{
	void *argp = NULL;

	if (_IOC_SIZE(request)) {
		va_list ap;
		va_start(ap, request);
		argp = va_arg(ap, void *);
		va_end(ap);
	}

	return syscall(3, fd, request, argp, SYS_IOCTL);
}
