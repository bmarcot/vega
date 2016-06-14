
	.syntax unified
	.thumb

#include "arch-v7m.h"
#include "linux/linkage.h"
#include "../asm-pseudo.S"

	.section ".vector", "a"

	.set THUMB_MODE, 1

	@ the vector must be aligned on 128 bytes (see VTOR reserved
	@ bits 6:0), alignment shall be handled in the linker script
	@ note: the SP has been set by the early vector at this point
	.long	__early_stack_start__		@ SP_main value
	.long	reset				@ Reset (warm)
	.long	0				@ NMI
	.long	hardf				@ HardFault
	//FIXME: Qemu does not escalate other faults to #hf
	.long	memf				@ MemManage
	.long	busf				@ BusFault
	.long	usgf				@ UsageFault
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
	orr	r2, #THUMB_MODE
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
	mov	r1, #0
	ldr	r2, =__bss_size__
	bl	memset

	@ The start_kernel function returns NULL or a pointer to the thread_info struct
	@ of the first thread to run on the CPU. If the pointer is NULL the CPU is
	@ software reseted (could be reseted differently). If the pointer is not NULL
	@ the first thread context is kickstarted. 
	bl	start_kernel	@ do the high-level kernel init
	cmp	r0, #0		@ the proc returns the first thread to run
	beq	system_reset	@ reset if system bootstrap failed
	b	thread_restore
ENDPROC(reset)

/* [1] http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHDGBJG.html */

	.macro	fault_handler f
	push	{r4-r11}	@ save non-scratch registers at time of fault
	mov	r0, sp
	mov	r2, lr		@ lr contains EXC_RETURN, fault was taken in thread or interrupt?

	@ We don't handle faults taken in interrupt handler at the moment.
	@ Hence the scratch registers auto pushed to the stack by the CPU
	@ on interrupt-entry have been pushed to PSP (MSP is only used by
	@ interrupts handler).
	mrs	r1, psp

	b	\f
	.endm	/* fault_handler */

ENTRY(hardf)
	fault_handler hardfault
ENDPROC(hardf)

ENTRY(memf)
	fault_handler memmanage
ENDPROC(memf)

ENTRY(busf)
	fault_handler busfault
ENDPROC(busf)

ENTRY(usgf)
	fault_handler usagefault
ENDPROC(usgf)

	.balign 16
