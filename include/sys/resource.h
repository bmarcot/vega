#ifndef SYS_RESOURCE_H
#define SYS_RESOURCE_H

typedef int rlim_t;

struct rlimit {
	rlim_t rlim_cur;  /* Soft limit */
	rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) */
};

int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);

#define RLIMIT_STACK 0

#endif /* !SYS_RESOURCE_H */
