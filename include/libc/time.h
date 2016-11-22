#ifndef LIBC_TIME_H
#define LIBC_TIME_H

#ifndef _TIME_T
#define _TIME_T
typedef unsigned int time_t;
#endif

struct timespec {
	time_t tv_sec;                /* Seconds */
	long   tv_nsec;               /* Nanoseconds */
};

struct itimerspec {
	struct timespec it_interval;  /* Timer interval */
	struct timespec it_value;     /* Initial expiration */
};


#endif /* !LIBC_TIME_H */
