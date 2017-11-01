#pragma once

#include <asm/posix_types.h>
#include <asm/syscalls.h>

int do_syscall0();
int do_syscall1();
int do_syscall2();
int do_syscall3();
int do_syscall4();
int do_syscall5();
int do_syscall6();

/* sched.h */
#define SYS_clone(...)		do_syscall4(__VA_ARGS__, SYS_CLONE)
#define SYS_execve(...)		do_syscall3(__VA_ARGS__, SYS_EXECVE)

/* linux/unistd.h */
#define SYS_exit_group(...)	do_syscall1(__VA_ARGS__, SYS_EXIT_GROUP)

/* linux/futex.h */
#define SYS_futex(...)		do_syscall3(__VA_ARGS__, SYS_FUTEX)

/* signal.h */
#define SYS_kill(...)		do_syscall2(__VA_ARGS__, SYS_KILL)
#define SYS_sched_yield(...)	do_syscall0(SYS_SCHED_YIELD)

/* sys/mman.h */
#define SYS_mmap(...)		do_syscall6(__VA_ARGS__, SYS_MMAP);
#define SYS_munmap(...)		do_syscall2(__VA_ARGS__, SYS_MUNMAP);

/* time.h */
#define SYS_nanosleep(...)	do_syscall2(__VA_ARGS__, SYS_NANOSLEEP);

/* unistd.h */
#define SYS_getpid(...)		do_syscall0(SYS_GETPID)
#define SYS_stat(...)		do_syscall2(__VA_ARGS__, SYS_STAT);
#define SYS_vfork(...)		do_syscall0(SYS_VFORK)

/* sys/wait.h */
#define SYS_waitpid(...)	do_syscall3(__VA_ARGS__, SYS_WAITPID)

/* fcntl.h */
#define SYS_open(...)		do_syscall2(__VA_ARGS__, SYS_OPEN);

/* non-standard calls */
#define _exit_thread(...)	(void)do_syscall1(__VA_ARGS__, SYS_EXIT)
#define SYS_sigreturn(...)	do_syscall0(SYS_SIGRETURN)
#define SYS_gettid(...)		do_syscall0(SYS_GETTID)
