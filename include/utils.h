#ifndef UTILS_H
#define UTILS_H

#define align_lo(val, align)	\
	((val) & ~((align) - 1))

#define align_hi(val, align)	\
	(((val) + (align) - 1) & ~((align) - 1))

#ifdef __GNUC__
#  define MAX(a, b)		 \
	({ __auto_type _a = (a); \
	   __auto_type _b = (b); \
	   _a > _b ? _a : _b; })
#  define MIN(a, b)		 \
	({ __auto_type _a = (a); \
	   __auto_type _b = (b); \
	   _a < _b ? _a : _b; })
#else
#  define MAX(a, b)  ((a) > (b) ? (a) : (b))
#  define MIN(a, b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef __LINKER__

#include "cmsis/arm/ARMCM4.h"

static inline void infinite_loop(void)
{
	for (;;)
		;
}

static inline void halt(void)
{
	for (;;)
		__WFI();
}

char *strrev(char *s);
void strpad(char *buf, char pad_val, int count);
char *_itoa(unsigned int u, char *buf, const char *base);

#endif /* !__LINKER__ */

#endif /* !UTILS_H */
