#ifndef UTILS_H
#define UTILS_H

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