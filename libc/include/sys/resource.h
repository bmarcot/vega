#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <asm/posix_types.h>

int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);

int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int prio);

#endif /* !_SYS_RESOURCE_H */
