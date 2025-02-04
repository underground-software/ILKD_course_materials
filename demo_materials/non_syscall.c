// syscall-free prime detector

.globl _start
_start:
	ldr x0, [sp]
	cmp x0, #2		// if (argc != 2)
	bne .Lbad		// 	goto bad;
	ldr x1, [sp, #16]	// char * x1 = argv[1];
	mov x0, #0		// int x0 = 0;
	mov x3, #10		// int x3 = 10;
.Ldigit_loop:
	ldrb w2, [x1], #1	// char w2 = *x1++;
	uxtw x2, w2		// long x2 = w2;
	cbz x2, .Lout		// if (x2 == '\0') goto prime_loop;
	mul x0, x0, x3		// x0 *= x3 /* 10 */;
	add x0, x0, x2		// x0 += x2;
	sub x0, x0, #'0'	// x0 -= '0' /* ASCII */;
	b .Ldigit_loop		// goto digit_loop;
.Lout:
	mov x2, #2		// int i = 2;
.Lprime_loop:
	cmp x2, x0		// if (i >= x0)
	bge .Lprime		// 	goto prime;
	udiv x3, x0, x2		// int quotient = x0/i;
	mul x3, x3, x2		// int nearest_multiple = quotient * i;
	sub x3, x0, x3		// int rem = x0 - nearest_multiple;
	add x2, x2, #1		// i++;
	cbnz x3, .Lprime_loop	// if (rem != 0) goto prime_loop;
.Lbad:
	msr VBAR_EL1, x0	// suicide by SIGILL
				// implements return false
.Lprime:
	mov x0, #0
	ldr x0, [x0]		// x0 = *NULL;
				// suicide by SIGSEGV
				// implements return true
