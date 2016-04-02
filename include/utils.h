#ifndef UTILS_H
#define UTILS_H

#define align_lo(val, align)	\
	((val) & ~((align) - 1))

#define align_hi(val, align)	\
	(((val) + (align) - 1) & ~((align) - 1))

#ifndef __LINKER__

static inline void infinite_loop(void)
{
	for (;;)
		;
}

//XXX: Use the ARM CMSIS instead?
static inline void __wfi(void)
{
	__asm__ __volatile__ ("wfi");
}

static inline void halt(void)
{
	for (;;)
		__wfi();
}

#endif /* !__LINKER__ */

#endif /* !UTILS_H */
