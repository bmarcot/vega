#include <pthread.h> //FIXME: Will die...
#include <stdlib.h>
#include <sys/pthread_types.h>

#include <kernel/syscalls.h>
#include "vega/syscalls.h"

#include "linux/list.h"

#include <vega/sys/mman.h>

#include <kernel/errno-base.h> //XXX: no kernel here, move to uapi, or to lib errno

//#include <sched.h> // the libc sched.h, with clone
int clone(int (*fn)(void *), void *child_stack,
	int flags, void *arg);

#define __STACK_ALLOCA(sp, off, ptr) ({			  \
	unsigned long __sp = (unsigned long)(sp) + (off); \
	*(ptr) = (__typeof__(*(ptr)))(__sp - sizeof(__typeof__(**(ptr)))); })

struct pthread {
	void             *arg;
	void             *(*start_routine)(void *);
	void             *stackaddr;
	unsigned long    stacksize;
	int              flags;
	_Atomic int      lock __attribute__((aligned(4)));
	struct pthread   *joiner;
	void             *retval;
	struct list_head list;
};

static LIST_HEAD(threads);

int __pthread_trampoline(void *);

#define PF_STACKALLOC 0x1
#define PF_DETACHED   0x2
#define PF_EXITING    0x4

#define IS_JOINED(pd)   ((pd)->joiner != NULL)
#define IS_DETACHED(pd) ((pd)->flags & PF_DETACHED)
#define IS_EXITING(pd)  ((pd)->flags & PF_EXITING)

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
	struct pthread *pthread;
	int flags = 0;
	void *stack;
	unsigned int stacksize;

	if (attr)
		stacksize = attr->stacksize;
	else
		stacksize = 512;

	if (!attr || !attr->stackaddr_set) {
		stack = mmap(NULL, stacksize, 0, MAP_ANONYMOUS, 0, 0);
		if (!stack)
			return EAGAIN;
		flags |= PF_STACKALLOC;
	} else {
		stack = attr->stackaddr;
	}

	__STACK_ALLOCA(stack, stacksize, &pthread);
	pthread->arg = arg;
	pthread->start_routine = start_routine;
	pthread->stackaddr = stack;
	pthread->stacksize = stacksize;
	pthread->flags = flags;
	pthread->lock = 0;
	pthread->joiner = NULL;
	pr_info("pthread_struct at %p", pthread);

	if (clone(__pthread_trampoline, pthread, 0, pthread) < 0) {
		if (flags & PF_STACKALLOC)
			munmap(stack, stacksize);
		return EAGAIN;
	}
	list_add(&pthread->list, &threads);
	*thread = (pthread_t)pthread;

	return 0;
}

static void release_pthread(struct pthread *pthread)
{
	pr_info("  releasae stack@%p", pthread->stackaddr);
	if (pthread->flags & PF_STACKALLOC)
		munmap((void *)pthread->stackaddr, pthread->stacksize);
	list_del(&pthread->list);
}

// syscall wrapper

#include <kernel/syscalls.h>
#include "vega/syscalls.h"

int SYS_futex(int *uaddr, int futex_op, int val)
{
	return do_syscall3((void *)uaddr, (void *)futex_op, (void *)val,
			SYS_FUTEX);
}

// !syscall wrapper

#include <uapi/kernel/futex.h>

void SYS_exit(int status);

void __pthread_exit(void *retval, struct pthread *pthread)
{
	pr_info("libpthread: exiting thread %p [stacksize=%p, flags=%x]",
		pthread, pthread->stacksize, pthread->flags);

	if (IS_DETACHED(pthread)) {
		pr_info("exiting a detached thread");
		release_pthread(pthread);
	} else {
		pthread->flags |= PF_EXITING;
		pthread->retval = retval;
		if (pthread->joiner)
			SYS_futex((int *)&pthread->lock, FUTEX_WAKE, 1);
	}
	SYS_exit((int)retval);
	/* if (list_is_singular(&threads)) */
	/* 	do_syscall1((void *)retval, SYS_EXIT); */
	/* else */
	/* 	do_syscall1((void *)retval, SYS_EXIT_GROUP); */
}

__attribute__(( always_inline )) __STATIC_INLINE uint32_t __get_SP(void)
{
	uint32_t result;

	__ASM volatile ("mov %0, sp\n" : "=r" (result));

	return result;
}

struct pthread *__pthread_self(void)
{
	struct pthread *pthread;
	unsigned long sp = __get_SP();

	//XXX: This is the dirty way to retrieve a pthread struct from the
	// current stack pointer. Another way could be using TLS (as ELF TLS,
	// or specific syscalls like get_tls() and set_tls()). Or find a user
	// RW register in Cortex-M that is not a Core register; but it does
	// not exist so far...
	list_for_each_entry(pthread, &threads, list) {
		if ((pthread->stackaddr <= (void *)sp)
			&& (pthread->stackaddr + pthread->stacksize > (void *)sp))
			break;
	}

	return pthread;
}

void pthread_exit(void *retval)
{
	__pthread_exit(retval, __pthread_self());
}

int pthread_join(pthread_t thread, void **retval)
{
	struct pthread *other = (struct pthread *)thread;

	/* Is thread joinable? */
	if (IS_DETACHED(other))
		return -EINVAL;

	/* Is somebody already waiting for that thread? */
	if (IS_JOINED(other))
		return -EINVAL;

	if (!IS_EXITING(other)) {
		other->joiner = __pthread_self();
		//pr_info("other lock = %p", &other->lock);
		SYS_futex((int *)&other->lock, FUTEX_WAIT, 0);
	}
	*retval = other->retval;
	release_pthread(other);

	return 0;
}

int pthread_detach(pthread_t thread)
{
	struct pthread *pthread = (struct pthread *)thread;

	pthread->flags |= PF_DETACHED;

	return 0;
}
