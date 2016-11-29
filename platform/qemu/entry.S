/*
 * platform/qemu/entry.S
 *
 * Copyright (C) 2016 Benoit Marcot
 */

	.syntax unified
	.thumb

#include "linux/linkage.h"

	/* void semih_exit(int status) */
ENTRY(semih_exit)
	tst	r0, r0
	ite	eq
	ldreq	r1, =0x20026    /* [a] */
	movne	r1, #0
	mov	r0, #0x18       /* [b] */
	bkpt	#0xab
0:	b	0b
ENDPROC(semih_exit)

/* [a] https://lists.nongnu.org/archive/html/qemu-devel/2014-12/msg01575.html
 *     ADP_Stopped_ApplicationExit is used for exit(0), anything else is
 *     implemented as exit(1). */

/* [b] http://git.qemu.org/?p=qemu.git;a=blob_plain;f=target-arm/arm-semi.c;hb=HEAD
 *     List of supported semihosting calls in Qemu. */
