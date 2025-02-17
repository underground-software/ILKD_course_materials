.macro macro_svc, number
	svc #\number
.endm

.section .text
.global _start
_start:
	mov x8, #64 	// write(2)
	mov x0, #1	// stdout
	ldr x1, =hello
	mov x2, #hello_len
	macro_svc 0
	mov x8, #94	// exit_group(2)
	mov x0, #0
	macro_svc 0
.section .data
hello:
	.ascii "Hello, world!\n"
hello_len = . - hello
