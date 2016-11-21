#ifndef KERNEL_H
#define KERNEL_H

/* Placeholder, will be used to gather init function in the same local memory
 *  area, and mitigate cache contention.    */
#define __init

/* Placeholder, such qualified pointers should be checked at runtime. */
#define __user

int printk(const char *fmt, ...);

/* stuff I don't know where to put */

/**
 * list_find_entry - iterate over list of given type
 * @pos:        the type * to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the list_head within the struct.
 * @value:      the value to compare against.
 * @field:      the name of the field to compare within the struct.
 */
/* https://gcc.gnu.org/onlinedocs/gcc/Local-Labels.html */
#define list_find_entry(pos, head, member, value, field)	\
	do {							\
		__label__ found;				\
		list_for_each_entry(pos, head, member) {	\
			if (pos->field == value)		\
				goto found;			\
		}						\
		if (&pos->member == head)			\
			pos = NULL;				\
	found:;							\
	} while (0);

/* round-down to a power of 2 */
#define align(x, a)          align_mask(x, (__typeof__(x))((a) - 1))
#define align_mask(x, mask)  ((x) & ~(mask))

/* round-up to a power of 2 */
#define align_next(x, a)          align_next_mask(x, (__typeof__(x))((a) - 1))
#define align_next_mask(x, mask)  (((x) + (mask)) & ~(mask))

#define max(a, b)			\
	({ __typeof__(a) _a = (a);	\
	   __typeof__(b) _b = (b);	\
	   _a > _b ? _a : _b; })

#define min(a, b)			\
	({ __typeof__(a) _a = (a);	\
	   __typeof__(b) _b = (b);	\
	   _a < _b ? _a : _b; })

#define ARRAY_SIZE(arr) \
	(sizeof(arr) / sizeof(*(arr)))

#define ARRAY_INDEX(elt, arr)				\
	({ unsigned int _elt = (unsigned int)(elt);	\
	   unsigned int _arr = (unsigned int)(arr);	\
	   (_elt - _arr) / sizeof(*(elt)); })

#endif /* !KERNEL_H */
