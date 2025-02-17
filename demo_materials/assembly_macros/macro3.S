.macro macro_svc, number
	svc #\number

.endm

.macro do_syscall, number
do_syscall_\()\number:
	mov x8, #\number
	macro_svc 0
	ret
.endm

do_syscall 64
do_syscall 94

#define FOO(bar) "Hello, " #bar "\n"

.section .text
.global _start
_start:
	//mov x8, #64 	// write(2)
	mov x0, #1	// stdout
	ldr x1, =hello
	mov x2, #hello_len
	bl do_syscall_64
	//mov x8, #94	// exit_group(2)
	mov x0, #0
	bl do_syscall_94
.section .data
hello:
	.ascii FOO(world)
hello_len = . - hello
