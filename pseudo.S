#ifndef ASM_PSEUDO_I
#define ASM_PSEUDO_I

	@ movl	rd, imm32
	.macro	movl rd, imm32
#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	.if (\imm32 >= 0) && (\imm32 <= 255)
	movs	\rd, \imm32
	.else
	ldr	\rd, =\imm32
	.endif
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	movw	\rd, #(\imm32 & 0xffff)
	.if (\imm32 < 0) || (\imm32 > 65535)
	movt	\rd, #(\imm32 >> 16)
	.endif
#endif
	.endm

	/* syscall call site */

	@ In order to not clobbering the parameter passing to function of the
	@ EABI, the SysCall ID is pushed as the last parameter in the param
	@ list of the called function. See the EABI specs to know which
	@ registers (and possibly the stack) are used during a function call.

#if __ARM_ARCH == 6 /* __ARM_ARCH_6M__ */
	@ sys	argc, id
#define DEFINE_SYS_CALL_SITE()			\
	.macro sys argc, id		;	\
	.if	\argc == 0		;	\
	movs	r0, #\id		;	\
	.elseif \argc == 1		;	\
	movs	r1, #\id		;	\
	.elseif \argc == 2		;	\
	movs	r2, #\id		;	\
	.elseif \argc == 3		;	\
	movs	r3, #\id		;	\
	.elseif \argc == 4		;	\
	push	{r0}			;	\
	movs	r0, #\id		;	\
	mov	r12, r0			;	\
	pop	{r0}			;	\
	.endif				;	\
	svc	#\argc			;	\
	.endm
#elif __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
	@ sys{cond} argc, id
#define DEFINE_SYS_CALL_SITE(cc)		\
	.macro sys##cc argc, id		;	\
	.if	\argc == 0		;	\
	mov##cc r0, #\id		;	\
	.elseif \argc == 1		;	\
	mov##cc r1, #\id		;	\
	.elseif \argc == 2		;	\
	mov##cc r2, #\id		;	\
	.elseif \argc == 3		;	\
	mov##cc r3, #\id		;	\
	.elseif \argc == 4		;	\
	mov##cc r12, #\id		;	\
	.endif				;	\
	svc##cc #\argc			;	\
	.endm
#endif

DEFINE_SYS_CALL_SITE();
#if __ARM_ARCH == 7 /* __ARM_ARCH_7M__ || __ARM_ARCH_7EM__ */
DEFINE_SYS_CALL_SITE(eq);
DEFINE_SYS_CALL_SITE(ne);
DEFINE_SYS_CALL_SITE(cs);
DEFINE_SYS_CALL_SITE(cc);
DEFINE_SYS_CALL_SITE(mi);
DEFINE_SYS_CALL_SITE(pl);
DEFINE_SYS_CALL_SITE(vs);
DEFINE_SYS_CALL_SITE(vc);
DEFINE_SYS_CALL_SITE(hi);
DEFINE_SYS_CALL_SITE(ls);
DEFINE_SYS_CALL_SITE(ge);
DEFINE_SYS_CALL_SITE(lt);
DEFINE_SYS_CALL_SITE(gt);
DEFINE_SYS_CALL_SITE(le);
#endif

#endif /* !ASM_PSEUDO_I */
