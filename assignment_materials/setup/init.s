.text
.global _start
_start:
	li a0, 1
	la a1, .Lmsg
	lui a2, %hi(.Lmsglen)
	addi a2, a2, %lo(.Lmsglen)
	li a7, 64	  # nr_write
	ecall
	li a0, 0xfee1dead # "feel dead"
	li a1, 0x28121969 # Linus Torvald's birthday
	li a2, 0x4321fedc # CMD_HALT
	li a7, 142	  # nr_rebbot
	ecall
	unimp
.data
.Lmsg:
.ascii "Hello!\n"
.Lmsglen = . - .Lmsg

