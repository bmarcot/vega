#pragma once

/* Include macros and defines used in specifying of an ioctl() request */
#include <uapi/kernel/ioctl.h>

int ioctl(int fd, unsigned long request, ...);
