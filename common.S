
	.syntax unified
	.thumb

#include "linux/linkage.h"

	@ offsets in the frame to the registers saved on interrupt-entry
	.set	R0, 0
	.set	RET_ADDRESS, 24

ENTRY(svcall)
	push	{r6, r7, lr}
	mrs	r6, psp
	ldr	r6, [r6, #RET_ADDRESS]
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	subs	r6, #2
	ldrb	r6, [r6]
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	ldrb	r6, [r6, #-2]
#endif
	ldr	r7, =syscall_vector
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	lsls	r6, #2
	ldr	r7, [r7, r6]
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	ldr.w	r7, [r7, r6, lsl #2]
#endif
	blx	r7
	mrs	r6, psp
	str	r0, [r6, #R0]
	pop	{r6, r7, pc}
ENDPROC(svcall)
