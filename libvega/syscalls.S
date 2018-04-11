/* Copyright (c) 2018 Benoit Marcot */

	.syntax unified
	.thumb

	.macro	_syscall argc
	.globl	_syscall\argc
_syscall\argc:
	svc	#\argc
	bx	lr
	.type	_syscall\argc, %function
	.size	_syscall\argc, . - _syscall\argc
	.endm

	.irpc	argc, 0123456
	_syscall \argc
	.endr
