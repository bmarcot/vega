#ifndef ASM_PSEUDO_I
#define ASM_PSEUDO_I

	@ context synchronization
	.macro	sync
	dsb	sy
	isb	sy
	.endm

	@ movl	rd, imm32
	.macro	movl rd, imm32
	movw	\rd, #(\imm32 & 0xffff)
	movt	\rd, #(\imm32 >> 16)
	.endm

	/* syscall call site */

	@ In order to not clobbering the parameter passing to function of the
	@ EABI, the SysCall ID is pushed as the last parameter in the param
	@ list of the called function. See the EABI specs to know which
	@ registers (and possibly the stack) are used during a function call.

	@ sys{cond}	argc, sys_id
#define DEFINE_SYS_CALL_SITE(cc)		\
	.macro sys##cc argc, id		;	\
	.if \argc == 0			;	\
	mov##cc r0, #\id		;	\
	.elseif \argc == 1		;	\
	mov##cc r1, #\id		;	\
	.elseif \argc == 2		;	\
	mov##cc r2, #\id		;	\
	.elseif \argc == 3		;	\
	mov##cc r3, #\id		;	\
	.elseif \argc == 4		;	\
	mov##cc r12, #\id		;	\
	.else				;	\
	.error "error: Unhandled number of parameters in SysCall." ;	\
	.endif				;	\
	svc##cc #\argc			;	\
	.endm

DEFINE_SYS_CALL_SITE();
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

#endif /* !ASM_PSEUDO_I */