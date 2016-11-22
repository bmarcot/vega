#ifndef LIBC_TIME_H
#define LIBC_TIME_H

#ifndef _CLOCKID_T
#define _CLOCKID_T
typedef unsigned int clockid_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef unsigned int time_t;
#endif

#ifndef _TIMER_T
#define _TIMER_T
typedef unsigned int timer_t;
#endif

struct timespec {
	time_t tv_sec;                /* Seconds */
	long   tv_nsec;               /* Nanoseconds */
};

struct itimerspec {
	struct timespec it_interval;  /* Timer interval */
	struct timespec it_value;     /* Initial expiration */
};

struct sigevent;

int timer_create(clockid_t clockid, struct sigevent *sevp,
		timer_t *timerid);

int timer_settime(timer_t timerid, int flags,
		const struct itimerspec *new_value,
		struct itimerspec *old_value);

#endif /* !LIBC_TIME_H */
