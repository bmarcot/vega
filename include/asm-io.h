#ifndef ASM_IO_H
#define ASM_IO_H

#include <sys/types.h>

/* Warning! Warning! Warning!
 *   These inline functions are correctly inlined iff `-O2' (at least) is passed
 *   to the compiler.  Observed on gcc version 5.3.1 20160307.
 */

static inline void writeb(u8 val, volatile void *addr)
{
	__asm__ __volatile__("strb %1, %0"
			:
			: "m" (*(volatile u8 *)addr), "r" (val));
}

static inline void writew(u16 val, volatile void *addr)
{
	__asm__ __volatile__("strh %1, %0"
			:
			: "m" (*(volatile u16 *)addr), "r" (val));
}

static inline void writel(u32 val, volatile void *addr)
{
	__asm__ __volatile__("str %1, %0"
			:
			: "m" (*(volatile u32 *)addr), "r" (val));
}

static inline u8 readb(const volatile void *addr)
{
	u8 val;
	__asm__ __volatile__("ldrb %0, %1"
			: "=r" (val)
			: "m" (*(volatile u8 *)addr));

	return val;
}

static inline u16 readw(const volatile void *addr)
{
	u16 val;
	__asm__ __volatile__("ldrh %0, %1"
			: "=r" (val)
			: "m" (*(volatile u16 *)addr));

	return val;
}

static inline u32 readl(const volatile void *addr)
{
	u32 val;
	__asm__ __volatile__("ldr %0, %1"
			: "=r" (val)
			: "m" (*(volatile u32 *)addr));

	return val;
}

#endif /* !ASM_IO_H */
