# 0 "macro4.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "macro4.S"


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



.section .text
.global _start
_start:

 mov x0, #1
 ldr x1, =hello
 mov x2, #hello_len
 bl do_syscall_64

 mov x0, #0
 bl do_syscall_94
.section .data
hello:
 .ascii "Hello, " "world" "\n"
hello_len = . - hello
