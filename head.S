
	.syntax unified
	.thumb

#include "arch-v7m.h"
#include "linux/linkage.h"
#include "../pseudo.S"

	.section ".vector", "a"

	.set THUMB_MODE, 1

	@ the vector must be aligned on 128 bytes (see VTOR reserved
	@ bits 6:0), alignment shall be handled in the linker script
	@ note: the SP has been set by the early vector at this point
	.long	__early_stack_start__		@ SP_main value
	.long	reset				@ Reset (warm)
	.long	0				@ NMI
	.long	hardf				@ HardFault
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	//FIXME: Qemu does not escalate other faults to #hf
	.long	memf				@ MemManage
	.long	busf				@ BusFault
	.long	usgf				@ UsageFault
#endif
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	0				@ Reserved
	.long	svcall				@ SVCall
	.long	0				@ Debug Monitor
	.long	0				@ Reserved
	.long	0				@ PendSV
	.long	systick				@ SysTick
	.long	0				@ External Interrupt
	//FIXME: interrupt handlers go there.

	.text

ENTRY(reset)
	movl	r0, V7M_SCB_BASE

	@ switch to Handler_Mode
	ldr	r1, =__early_stack_end__	/* allocate a temporary vector */
	str	r1, [r0, #V7M_VTOR]
	ldr	r2, =0f
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	movs	r3, #THUMB_MODE
	orrs	r2, r3
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	orr	r2, #THUMB_MODE
#endif
	str	r2, [r1, #V7M_VTOR_OFFSET_SVC]
	dsb					/* [1] */
	svc	#0
0:

	@ restore the early vector
	ldr	r1, =__vector_start__
	str	r1, [r0, #V7M_VTOR]
	dsb					/* [1] */

	@ Copy the .data section from Flash to RAM.
	ldr	r0, =__data_start__
	ldr	r1, =__rodata_end__
	ldr	r2, =__data_size__
	bl	memcpy

	@ Zero-fill the .bss sections.
	ldr	r0, =__bss_start__
	movs	r1, #0
	ldr	r2, =__bss_size__
	bl	memset

	@ The start_kernel function returns NULL or a pointer to the thread_info struct
	@ of the first thread to run on the CPU. If the pointer is NULL the CPU is
	@ software reseted (could be reseted differently). If the pointer is not NULL
	@ the first thread context is kickstarted. 
	bl	start_kernel	@ do the high-level kernel init
	cmp	r0, #0		@ the proc returns the first thread to run
	beq	0f		@ reset if system bootstrap failed
	b	thread_restore
0:	ldr	r0, =system_reset
	bx	r0
ENDPROC(reset)

/* [1] http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHDGBJG.html */

	.balign 16
