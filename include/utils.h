#ifndef UTILS_H
#define UTILS_H

#define align_lo(val, align)	\
	((val) & ~((align) - 1))

#define align_hi(val, align)	\
	(((val) + (align) - 1) & ~((align) - 1))

#endif /* !UTILS_H */
