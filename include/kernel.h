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

#endif /* !KERNEL_H */
