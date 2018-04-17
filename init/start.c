#include "../libsemi/v7m_semi.h"
#include "../libvega/syscalls.h"

#include "../arch/arm/include/uapi/asm/syscalls.h"
#include "../arch/arm/include/uapi/asm/ptrace.h"

#include "../include/uapi/kernel/mman.h"
#include "../include/uapi/kernel/sched.h"

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	return (void *)syscall(6, addr, length, prot, flags, fd, offset, SYS_MMAP);
}

int clone(unsigned long flags, void *child_stack, struct pt_regs *regs)
{
	return syscall(3, flags, child_stack, regs, SYS_CLONE);
}

int sched_yield(void)
{
	return syscall(0, SYS_SCHED_YIELD);
}

int _start(unsigned int main_ptr)
{
	char *stack = mmap(0, 512, 0, MAP_ANONYMOUS, 0, 0);
	struct pt_regs regs;

	regs.pc = main_ptr;
	clone(0, stack + 512, &regs);

	while (1)
		sched_yield();
}
