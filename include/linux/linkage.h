#ifndef LINKAGE_H
#define LINKAGE_H

/*
 * found here:
 *   http://www.cs.fsu.edu/~baker/devices/lxr/http/source/linux/include/linux/linkage.h
 */

#define ENTRY(name)	\
	.globl name;	\
	.align 4;	\
name:

#define END(name)	\
	.size name, .-name

#define ENDPROC(name)		\
	.type name, %function;	\
	END(name)

#endif /* !LINKAGE_H */
