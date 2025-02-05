# Syscalls

---

### Learning Objective

Understand the story of a system call

---

# What is a syscall?

1. A userspace request for kernel action

1. The mechanism to escalate privileges

1. The terms "syscall" and "system call" are used interchangably

---

# demo: this riscv program will KILL you

```
.text
.globl _start
_start:
csrr a0, mtvec
```

---

# [riscv privileged isa releases](https://github.com/riscv/riscv-isa-manual/releases)

---

# demo

this arm64 program will KILL you

```
.text
.globl _start
_start:
msr VBAR_EL1, x0
```

---

# Overview

1. Execution contexts

1. Define kernelspace and userspace

1. Kernel representation of a process or thread

1. What do we want out of a system call?

1. The five steps of a system call

---

# Execution context

An execution context is a CPU register state

---

# Execution context example 1

The `{set,long}jmp(3)` library functions

1. `setjmp`: save current state

1. `longjmp`: restore saved register state

---

# demo

example use of `{set,long}jmp`

---

# Execution context example 2

Threads in a program

1. Threads share: address space (heap, code, static data)

1. Threads have their own: stack (register ptr) registers, IP

---

# Kernelspace and userspace

Kernelspace and userspace are distinct execution contexts

* Primary difference: privilege level

---

# Kernelspace and userspace

Normal programs run in userspace

Kernelspace is the privileged execution context

1. Registers, stack, memory are familiar

1. Key difference: CPU capabilities are unrestricted

---

# Kernelspace simplification

Kernelspace execution context can be further subdivided

1. Kernel code may be running on behalf of a particular userspace process

1. Other kernel code runs on its own behalf

---

# Definition of a context switch

1. A capture of the CPU register state

1. A load of a saved CPU register state

---

# Context switching and the kernel

Switching to kernel context is like other context switches

1. Main difference: privileges escalated

1. Switching back to userspace is similar but privilegs are dropped

---

# Note on terminology

The term "context switch" is sometimes used to refer to task-switching

1. Each task-switch involves several context switches

---

# Definition of re-entrancy

"A computer program or subroutine is called reentrant if multiple invocations can safely run concurrently on multiple processors" ([source](https://en.wikipedia.org/wiki/Reentrancy_(computing)))

1. Important concept for kernel code

---

# Introducing the `struct task_struct`

 This is Linux's [Process Control Block (PCB)](https://en.wikipedia.org/wiki/Process_control_block)

1. each kernel `pid` has unique `struct task_struct`

---

# demo

A quick look at `include/linux/sched.h`

---

# Introducing the `current` macro

Refers to `struct task_struct` of process in current execution context

---

# demo

A quick look at three files:

1. riscv64: [arch/riscv/include/asm/current.h](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/include/asm/current.h)

1. arm64: [arch/arm64/include/asm/current.h](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/include/asm/current.h)

1. x86: [arch/x86/include/asm/current.h](https://elixir.bootlin.com/linux/v6.13/source/arch/x86/include/asm/current.h)

---

# The meaning of `pid`

|kernelspace name|userspace name|
|--|--|
|pid|tid|
|tgid|pid|

---

# demo
       
see `get{p,t}id(2)` in `kernel/sys.c`

---

# Simplified `getpid(2)` call stack

```
getpid(2) calls functions

... namespaces are taken into account

... locking is done

task_pid_nr() { ... tsk->pid ... }
```

---

# The `pid`/`tgid` distinction

Why do we have different names?

---

# History of `tgid`: the dilema

Before Linux 2.6, there were only `pid`s

1. The `clone(2)` call could share address space between processes

1. This allowed thread-like behavior

1. These processes were too independent

    1. Example: no shared signals

---

# History of `tgid`: the proposal

[NPTL](https://en.wikipedia.org/wiki/Native_POSIX_Thread_Library) implements threads as specified by POSIX

1. This required both userspace and kernelspace changes

---

# History of `tgid`: userspace changes

1. The C library was hardened for concurrency

1. The C library introduced the `tid` concept

1. The `tid` subdivides a `pid`

---

# History of `tgid`: kernelspace changes

1. The kernel introduced the `tgid` concept

1. The `tgid` groups kernel `pid`s together

---

# History of `tgid`: present day

Each `pid` corresponds to unique `struct task_struct`

1. The `tgid` and `pid` values are stored here

---

# Syscalls: are they necessary?

Can a program do anything useful without making any syscalls?

---

# Syscalls: highly necessary

All useful programs depend on system calls

---

# demo

Let's trace a program's syscall usage with strace

---

# demo

Syscall-free prime-number detector program

---

# Desirable properties of syscalls

1. speed

1. security

1. stability

1. re-entrancy

---

# Security concerns

1. [confused deputy problem](https://en.wikipedia.org/wiki/Confused_deputy_problem)

1. One example: validate address range of any pointer arguments

---

# Stability

Linux provides a stable syscall API

1. This sets Linux apart from other OSes

---

# Syscall implementation

A syscall can be broken down into 5 distinct steps

1. Userspace invocation

1. Hardware-assisted privilege escalation

1. Kernel code handler

1. Hardware-assisted privilege drop

1. Userspace program continues

The transfer of software or hardware responsibility divides each step

---

# Userspace invocation

All programs make system calls

1. excluding trivial example programs

Example: a shell as an abstraction over many syscalls

---

# demo

see `/proc/PID/syscall`

1. [Multi-arch syscall number table](https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html)

---

# Userspace invocation: library wrappers

The C library provides wrapper functions for many syscalls

1. Main benefit: speed

1. We want to minimize the high-overhead syscalls

1. Checks like input validation avoid syscalls if possible

1. Avoid architecture specific details

---

# Common notation: manual page section numbers

Example: `write(2)` vs `write(3)`

1. Number in parenthesis refers to manual page section number

1. Section 2 has system calls and section 3 has library calls

See `man man` for more information

---

# demo

ltrace: like strace for library calls

---

# Userspace invocation: architecture-specific

Common accross architectures:

1. specify the syscall and arguments

1. give up control to the hardware

---

# Userspace invocation: riscv


1. Specify syscall number in `a7`

1. Specify arguments 1-6 in `a0`, `a1`, `a2`, `a3`, `a4`, `a5`

1. Return value will be in `a0`

1. `ecall` gives up control to hardware
---

# Userspace invocation: arm64

1. Specify syscall number in `x8`

1. Specify arguments 1-6 in `x0`, `x1`, `x2`, `x3`, `x4`, `x5`

1. Return value will land in `x0`

1. `svc #0` gives up control to hardware

---

# Userspace invocation: x86_64

1. Specify syscall number in `rax`

1. Args 1-6 in: `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`

1. `syscall` gives up control to hardware

1. Return value will land in `rax`

---

# Userspace invocation: x86_64 fine print

Difference from normal function calling convention

1. The `syscall` instruction clobbers `rcx`

1. Use `r10` instead of `rcx`

---

# Userspace invocation: wrap up

With arguments chosen and syscall selected

1. Give up control to the hardware

---

# Hardware-assisted privilege escalation

This step is handled by hardware

1. How does hardware know what to do?

---

# Hardware-assisted privilege escalation

Rewind to boot

1. Linux installs it's syscalls into our CPU

---

# Hardware-assisted privilege escalation: riscv

See `_start`:

1. Set [`CSR_TVEC`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/kernel/head.S#L186) to address of `handle_exception()`

1. [`CSR_TVEC`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/include/asm/csr.h#L444): Control and Status Register: Trap Vector Base Address Register

---

# Hardware-assisted privilege escalation: arm64

See `__primary_switched()`

1. [Set `VBAR_EL1`](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/head.S#L225) to address of vector table

1. Vector table defined in [entry.S](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/entry.S#L521)

1. VBAR_EL1: Vector Base Address Register (Exception Level 1)

---

# Hardware-assisted privilege escalation: x86_64

See `syscall_init()`

1. [Set MSR_LSTAR](https://elixir.bootlin.com/linux/v6.13/source/arch/x86/kernel/cpu/common.c#L2041) to `entry_SYSCALL_64` address

1. MSR_LSTAR: Model Specific Register: Long System Target Address Register

---

# Hardware-assisted privilege escalation: at invocation

Back to the present

1. The CPU is preconfigured to correctly transfer control

1. This makes privilege escalation safe

---

# Hardware-assisted privilege escalation

1. On riscv: switch to machine mode

1. On arm64: elevate execution level

1. On x86_64: change to ring 0

1. Both of these are stored in a particular register

---

# Kernel handles request

1. Part architecture-specific

1. Part architecture-generic

---

# Kernel handles request: starting point

Execution resumes from a hardware specified register rate

1. At bottom, mostly assembly and C macros

1. Higher on call stack is more generic code

---

# Kernel handles request: `riscv`

Start in `handle_exception`

1. Assembly jumps into a particular offset in [`excp_vect_table`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/kernel/entry.S#L440)

---

# Kernel handles request: `riscv`

Get the syscall number in [`do_trap_ecall_u()`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/kernel/traps.c#L318)

1. Index into the [`sys_call_table`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/include/asm/syscall.h#L88) array and call the function

---

# Kernel handles request: `riscv`

[`syscall_table_64.h`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/include/asm/Kbuild) generated at build time

1. [An included Makefile](https://elixir.bootlin.com/linux/v6.13/source/scripts/Makefile.asm-headers) generates this from a common table

1. [`scripts/syscalls.tbl`](https://elixir.bootlin.com/linux/v6.13/source/scripts/syscall.tbl) enumerates the syscalls and compatibility information

---

# Kernel handles request: `arm64`

Start in `VBAR_EL1`

1. Hardware jumps to particular offset in [`vectors`](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/entry.S#L521)

---

# Kernel handles request: `arm64` reaches C

A function defined by macro in `entry.S` calls into `C` code

1. The first C function is [el0t_64_sync_handler()](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/entry-common.c#L756)

---

# Kernel handles request: `arm64` goes geenric

Execution reaches [`el0_svc_common()`](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/syscall.c#L73)

1. The [`invoke_syscall()`](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/syscall.c#L38) indexes into jump table of handlers

1. This architecture-generic handler is defined by a `SYSCALL_DEFINE*` macro

---

# Kernel handles request: `x86_64` entry

Start at[`entry_SYSCALL_64`](https://elixir.bootlin.com/linux/v6.13/source/arch/x86/entry/entry_64.S#L87)

1. Assembly calls into the [`do_syscall_64()`](https://elixir.bootlin.com/linux/v6.13/source/arch/x86/entry/common.c#L76) C function

---

# Kernel handles request: `x86_64` goes generic

Using a few helper functions, index into jump table of system call handlers

1. This architecture-generic handler is defined by a `SYSCALL_DEFINE*` macro

---

# Further reading on `x86_64` syscall implementation details

1. We have another [article](https://kdlp.underground.software/articles/syscalls_end_to_end.html) about this available

---

# Kernel handles request: architecture-generic

A closer look at the `SYSCALL_DEFINE*()` handlers

---

# Kernel handles request: `SYSCALL_DEFINE_*`

Defined in `include/linux/syscalls.h`

1. Resolve to `__SYSCALL_DEFINEx(x,...`

1. Five functions generated

1. See `__do_sys##name(...`

---

# A note on syscall arguments

No SYSCALL_DEFINE7 and above

---

# demo

Take a look at the `SYSCALL_DEFINE` macro definition in [`include/linux/syscalls.h`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/syscalls.h#L233)

---

# Kernel handles request: return imminent


Indicate error using the errno macros

Return to assembly for another context switch

---

# Kernel handles request: riscv returns

`do_trap_ecall_u()` returns to assembly code

1. After finishing the function found in `excp_vect_table`, [jump to `ret_from_exception`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/kernel/entry.S#L198)

1. [Place userspace program counter in `mepc`: machine exception program counter CSR](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/kernel/entry.S#L266)

1. [`mret`](https://elixir.bootlin.com/linux/v6.13/source/arch/riscv/kernel/entry.S#L277) gives up control to the hardware

1. `sret` used in supervisor mode while `mret` used in machine mode

---

# Kernel handles request: arm64 returns
    
[`el0t_64_sync()`](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/entry.S#L580) calls `ret_to_user()`

1. Which calls `kernel_exit 0`, defined as a [macro](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/entry.S#L337)

1. [`eret`](https://elixir.bootlin.com/linux/v6.13/source/arch/arm64/kernel/entry.S#L463) gives up control to the hardware once again

---

# Kernel handles request: x86_64 returns

`entry_SYSCALL_64()` prepares to return

1. First [check](https://elixir.bootlin.com/linux/v6.13/source/arch/x86/entry/common.c#L102) whether we can use the faster `sysret` instruction

1. [`sysret`](https://www.felixcloutier.com/x86/sysret) is faster than [`iret`](https://www.felixcloutier.com/x86/iret:iretd:iretq)

1. Via either instruction we give up control to the hardware once again

---

# Hardware-assisted privilege drop

Less dangerous operation than escalation

1. Restore old register and stack

1. Drop privileges

1. Set program counter to userspace return address

---

# Hardware-assisted privilege drop: riscv

The `ecall` instruction places the return address in the `mepc` control and status register (in m-mode)

The `mret` instuction sets the program counter to the value in `mepc`

---

# Hardware-assisted privilege drop: arm64

The `svc #0` instruction saves a return address in hardware

The `eret` instruction sets the program counter to this value

---

# Hardware-assisted privilege drop: x86_64

[`iret`](https://www.felixcloutier.com/x86/iret:iretd:iretq) loads the return address form the stack

[`sysret`](https://www.felixcloutier.com/x86/sysret) returns to `rcx`

---

# Hardware-assisted privilege drop: completed

Software takes control of execution

---

# Userspace program continues

Always check for an error

---

# Userspace program continues: errno

Kernel functions return `-errno`

C library wrappers check for error

1. Store original error in `errno`

1. Convert return code to `-1`

1. Example: [musl syscall return](https://elixir.bootlin.com/musl/latest/source/src/internal/syscall_ret.c#L4)

---

# Demo:

The `errno` utility from `moreutils` package

---

# errno further reading

See `man 3 errno`

---

# Userspace program continues

The system call is complete

---

# The story of a system call: A summary

Linux provides a stable system call API

---

# The story of a system call: A summary

1. Most programs run in user execution context ("userspace")

1. Kernel code runs in several execution contexts (all "kernelspace")

---

# The story of a system call: A summary

Hardware plays two key roles in system calls

1. Raising privileges and entering kernel execution context

1. Dropping privileges and entering user execution context

---

# The story of a system call: A summary

Many syscall implementation details are architecture-specific

---

# The story of a system call: A summary

The kernel defines the main syscall handler using a `SYSCALL_DEFINE*` macro

1. These macros are used to define system call implementations

---

# The story of a system call: A summary

The C library defines wrapper functions for many syscalls

1. These hide architecture-specific details

1. Provide POSIX-compatible behavior by hiding Linux eccentricities

---

# The story of a system call: A summary

Always check for an error after making a syscall

--- 

# End
