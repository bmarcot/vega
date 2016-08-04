
#include "linux/linkage.h"

	.syntax	unified
	.thumb

	@ offsets in the frame to the registers saved on interrupt-entry
	.set	R0, 0
	.set	RET_ADDRESS, 24

	.global	syscall_return

ENTRY(svcall)
	push	{r6, r7, lr}
	ldr	r7, =syscall_vector
	mrs	r6, psp
	ldr	r6, [r6, #RET_ADDRESS]
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	subs	r6, #2
	ldrb	r6, [r6]
	lsls	r6, #2
	ldr	r7, [r7, r6]
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	ldrb	r6, [r6, #-2]
	ldr.w	r7, [r7, r6, lsl #2]
#endif
	blx	r7
syscall_return:
	mrs	r6, psp
	str	r0, [r6, #R0]
	pop	{r6, r7, pc}
ENDPROC(svcall)
