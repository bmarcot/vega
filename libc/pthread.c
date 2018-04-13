#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/pthread_types.h>

#include <kernel/list.h>
#include <asm/syscalls.h>
#include <libvega/syscalls.h>

#include <uapi/kernel/sched.h>

#include <asm/ptrace.h>

#include <kernel/errno-base.h> //XXX: no kernel here, move to uapi, or to lib errno

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
#ifdef DBG_PTHREAD
	pr_info("thread@%p", pthread);
#endif

	struct pt_regs regs = {
		.r0 = (u32)pthread,
		.pc = (u32)__pthread_trampoline,
	};
	int r = clone(CLONE_THREAD | CLONE_SIGHAND, pthread, &regs);
	if (r < 0) {
		if (flags & PF_STACKALLOC)
			munmap(stack, stacksize);
		return EAGAIN;
	}
	list_add(&pthread->list, &threads);
	*thread = (pthread_t)pthread;

	return 0;
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

pthread_t pthread_self(void)
{
	return (pthread_t)__pthread_self();
}

static void release_pthread(struct pthread *pthread)
{
	if (pthread->flags & PF_STACKALLOC)
		munmap((void *)pthread->stackaddr, pthread->stacksize);
	list_del(&pthread->list);
}

#include <uapi/kernel/futex.h>

static inline int futex(int *uaddr, int futex_op, int val)
{
	return syscall(3, uaddr, futex_op, val, SYS_FUTEX);
}

void __pthread_exit(void *retval, struct pthread *pthread)
{
#ifdef DBG_PTHREAD
	pr_info("thread@%p, stacksize=0x%x, flags=%x",
		pthread, pthread->stacksize, pthread->flags);
#endif

	if (IS_DETACHED(pthread)) {
		release_pthread(pthread);
	} else {
		pthread->flags |= PF_EXITING;
		pthread->retval = retval;
		if (pthread->joiner)
			futex((int *)&pthread->lock, FUTEX_WAKE, 1);
	}
	syscall(1, retval, SYS_EXIT);

	/* if (list_is_singular(&threads)) */
	/* 	do_syscall1((void *)retval, SYS_EXIT); */
	/* else */
	/* 	do_syscall1((void *)retval, SYS_EXIT_GROUP); */
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
		futex((int *)&other->lock, FUTEX_WAIT, 0);
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

int pthread_yield(void)
{
	return sched_yield();
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
	attr->stacksize = stacksize;

	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
	*stacksize = attr->stacksize;

	return 0;
}

int pthread_attr_init(pthread_attr_t *attr)
{
	attr->stackaddr_set = 0;
	attr->stacksize = 512;

	return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	(void)attr;

	mutex->__lock = PTHREAD_MUTEX_UNLOCKED;

	return 0;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
	(void)attr;

	cond->__lock = PTHREAD_MUTEX_UNLOCKED;
	cond->__futex = 0;

	return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	__pthread_mutex_lock(&cond->__lock);
	pthread_mutex_unlock(mutex);
	int futex_val = cond->__futex;
	cond->__nwaiters++;
	__pthread_mutex_unlock(&cond->__lock);
	futex(&cond->__futex, FUTEX_WAIT, futex_val);

	return pthread_mutex_lock(mutex);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
	__pthread_mutex_lock(&cond->__lock);
	if (cond->__nwaiters) {
		cond->__nwaiters--;
		cond->__futex++;
		futex(&cond->__futex, FUTEX_WAKE, 1);
	}
	__pthread_mutex_unlock(&cond->__lock);

	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return __pthread_mutex_lock(&mutex->__lock);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	return __pthread_mutex_trylock(&mutex->__lock);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return __pthread_mutex_unlock(&mutex->__lock);
}
