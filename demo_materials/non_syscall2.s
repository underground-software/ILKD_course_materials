    .globl _start
_start:
    # Load argc
    lw a0, 0(sp)
    li a1, 2          	# if (argc != 2)
    bne a0, a1, bad   	# 	goto bad;

    ld a1, 16(sp)     	# char *a1 = argv[1];

    li a0, 0          	# int a0 = 0
    li a3, 10         	# int a3 = 10

digit_loop:
    lbu a2, (a1)       	# char a2 = *a1
    addi a1, a1, 1    	# a1++
    beqz a2, prime_loop # if (a2 == '\0') goto prime_loop

    mul a0, a0, a3    	# a0 *= 10
    add a0, a0, a2    	# a0 += a2
    addi a0, a0, -'0'  	# a0 -= '0' (ASCII)
    j digit_loop      	# goto digit_loop

prime_loop:
    li a2, 2          	# int i = 2

prime_test:
    bge a2, a0, prime # if (i >= a0) goto prime

    remu a3, a0, a2	# int rem = a0 % i

    addi a2, a2, 1    	# i++
    bnez a3, prime_test # if (rem != 0) goto prime_test

bad:
    unimp             	# Trigger illegal instruction (SIGILL)

prime:
    lw a0, (zero)      	# *0; Cause segmentation fault (SIGSEGV)
