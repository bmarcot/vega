/* Copyright (c) 2018 Benoit Marcot */

	.syntax unified
	.thumb

#define ENTRY(name)		\
	.globl name;		\
name:

#define ENDPROC(name)		\
	.type name, %function;	\
	.size name, . - name

/* http://git.qemu.org target/arm/arm-semi.c
 * List of supported semihosting calls in Qemu */
#define TARGET_SYS_WRITEC		0x03
#define TARGET_SYS_WRITE0		0x04
#define TARGET_SYS_EXIT			0x18

ENTRY(v7m_semi_writec)
	mov	r1, r0
	mov	r0, #TARGET_SYS_WRITEC
	bkpt	#0xab
	bx	lr
ENDPROC(v7m_semi_writec)

ENTRY(v7m_semi_write0)
	mov	r1, r0
	mov	r0, #TARGET_SYS_WRITE0
	bkpt	#0xab
	bx	lr
ENDPROC(v7m_semi_write0)

/* ADP_Stopped_ApplicationExit is used for exit(0),
 * anything else is implemented as exit(1) */
#define ADP_Stopped_ApplicationExit	0x20026

ENTRY(v7m_semi_exit)
	tst	r0, r0
	ite	eq
	ldreq	r1, =ADP_Stopped_ApplicationExit
	movne	r1, #0
	mov	r0, #TARGET_SYS_EXIT
	bkpt	#0xab
0:	b	0b
ENDPROC(v7m_semi_exit)
