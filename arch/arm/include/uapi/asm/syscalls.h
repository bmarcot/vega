//XXX: This file is not really needed by the kernel and
// is rather part of the API documentation

#ifndef _ASM_SYSCALLS_H
#define _ASM_SYSCALLS_H

#define SYS_TIMER_CREATE 0
#define SYS_TIMER_SETTIME 1
#define SYS_TIMER_GETTIME 2
#define SYS_NANOSLEEP 3
#define SYS_EXECVE 4
#define SYS_VFORK 5
#define SYS_SIGACTION 6
#define SYS_KILL 7
#define SYS_SIGQUEUE 8
#define SYS_OPEN 9
#define SYS_CLOSE 10
#define SYS_READ 11
#define SYS_WRITE 12
#define SYS_LSEEK 13
#define SYS_STAT 14
#define SYS_MOUNT 15
#define SYS_READDIR_R 16
#define SYS_GETPID 17
#define SYS_MMAP 18
#define SYS_MUNMAP 19
#define SYS_CLONE 20
#define SYS_EXIT 21
#define SYS_FUTEX 22
#define SYS_SCHED_YIELD 23
#define SYS_GETPRIORITY 24
#define SYS_SETPRIORITY 25
#define SYS_SIGRETURN 26
#define SYS_CLOCK_GETTIME 27
#define SYS_EXIT_GROUP 28
#define SYS_WAITPID 29
#define SYS_GETTID 30
#define SYS_GETPPID 31
#define SYS_TGKILL 32

#endif /* !_ASM_SYSCALLS_H */
