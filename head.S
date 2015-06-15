	.syntax unified
	.thumb

	.global	_start
	.set	THUMB_MODE,	1

	.text

vector:
	.word	0x20004000	@ FIXME: stack base on lm3s6965evb
	.word	(_start + THUMB_MODE)

_start:				@ FIXME: rename as reset handler
	mov	r0, #5		@ FIXME: dummy instructions...
	mov	r1, #4
	add	r2, r1, r0
	b	start_kernel
