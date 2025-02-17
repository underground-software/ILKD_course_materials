# 0 "macro2.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "macro2.S"
.macro macro_svc, number
 svc #\number
.endm



.section .text
.global _start
_start:
 mov x8, #64
 mov x0, #1
 ldr x1, =hello
 mov x2, #hello_len
 macro_svc 0
 mov x8, #94
 mov x0, #0
 macro_svc 0
.section .data
hello:
 .ascii "Hello, " "world" "\n"
hello_len = . - hello
