# Syscalls

### Learning Objective: Understand the story of a system call

1. What is a syscall?

    1. A userspace request for kernel action

    1. The mechanism to escalate privileges

    1. Syscall and system call used interchangably

    1. demo: this arm64 program will KILL you (arm64_priv)

```
.text
.globl _start
_start:
msr VBAR_EL1, x0
```

1. Overview

    1. Execution contexts

    1. Define kernelspace and userspace

    1. Kernel representation of a process or thread

    1. What do we want out of a system call?

    1. The five steps of a system call

1. Background information

    1. What is execution context?

        1. An execution context is a CPU register state

        1. First example: `{set,long}jmp(3)` library functions

            1. setjmp: save current state

            1. longjmp: restore saved register state

            1. demo: see simple setjmp/longjmp pair (setjmp_longjmp)

        1. Threads in a program

            1. threads share: address space (heap, code, static data)

            1. threads have their own: stack (register ptr) registers, IP

    1. Kernelspace and userspace are distinct execution contexts

        1. Primary difference: privilege level

        1. Different types of context difference than thread vs thread

        1. Normal programs run in userspace

        1. Kernelspace is the privileged execution context

            1. Registers, stack, memory are familiar

            1. Key difference: CPU capabilities are unrestricted

        1. Kernelspace can be further subdivided into contexts

            1. Kernel code may be running on behalf of a particular userspace process

            1. Other kernel code runs on its own behalf

    1. Define context switch and it's relation to syscalls

        1. A change of CPU register state

        1. Some may be familiar with process to process context switches

        1. Each of these can be broken down further

        1. First, the transition from userspace to kernelspace

        1. Save registers and switch stack, raise privileges

        1. Second, the transition from kernelspace to userspace

        1. Registers and switch stack, drop privileges

        1. We will refer to each of these distinct transitions
        as a "context switch"

    1. Important kernel code for syscall discussion

        1. What is the `struct task_struct`

        1. This is Linux's [Process Control Block (PCB)](https://en.wikipedia.org/wiki/Process_control_block)

        1. each pid has unique struct task_struct

        1. demo: see include/linux/sched.h


        1. current macro refers to `struct task_struct` of process in this context

        1. arm64: arch/arm64/include/asm/current.h

        1. x86: arch/x86/include/asm/current.h

        1. pid in kernel vs userspace

            |kernelspace name|userspace name|
            |--|--|
            |pid|tid|
            |tgid|pid|

        1. demo: see `get{p,t}id(2)` in kernel/sys.c

        1. simplified call stack

            1. `task_tgid_vnr() in include/linux/pid.h`

            1. `... (namespaces are taken into account): return to this later`
            1. `... locking: return to this later`

            1. `task_pid_nr() { ... tsk->pid ... }`

        1. Before Linux 2.6, there were only `pid`s

            1. The `clone(2)` call could share address space between processes

                1. This allowed thread-like behavior

                1. These processes were too independent

                1. e.g. no shared signals

            1. [NPTL](https://en.wikipedia.org/wiki/Native_POSIX_Thread_Library) implements threads as specified by POSIX

                1. This required both userspace and kernelspace changes

                    1. The C library was hardened with locking

                    1. The C library introduced the `tid` concept

                    1. The `tid` subdivides a `pid`

                1. On the kernel side:

                    1. The kernel introduced the `tgid` concept

                    1. The `tgid` groups kernel `pid`s together

                    1. Each `pid` corresponds to unique `struct task_struct`
                    1. The `tgid` and `pid` values are stored here

1. All useful programs depend on system calls

    1. demo: Let's trace a program's syscall usage with strace

    1. demo: a non-trivial syscall-free program: detect prime numbers (non_syscall)

1. What do we want out of syscalls?

    1. We need speed

    1. We want re-entrancy

        1. Interupt/resume execution of any number of concurrent threads

    1. We need security

        1. e.g. we validate ptr args in expected addr range

        1. confused deputy problem [1]

    1. Linux provides a stable syscall API

        1. This sets Linux apart from other OSes

1. How does Linux implement syscalls?

    1. A syscall can be broken down into 5 distinct steps

        1. Userspace invocation

        1. Hardware-assisted privilege escalation

        1. Kernel code handler

        1. Hardware-assisted privilege drop

        1. Userspace program continues

1. The transfer of software or hardware responsibility divides each step

1. Userspace program preparation and invocation

    1. All programs make system calls

        1. excluding trivial example programs

    1. e.g. a shell as an abstraction over many syscalls

    1. demo: see /proc/PID/syscall, e.g. pid 1

        1. [Multi-arch syscall number table](https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html)

    1. when possible prefer library-provided syscall wrapper

        1. glibc provides wrapper functions for many syscalls

            1. Main benefit: speed

            1. We want to minimize the high-overhead syscalls

            1. Checks like input validation avoid syscalls if possible

        1. We use e.g. write(3) instead of write(2)

        1. Common manual number section notation

            1. e.g. write(2)

            1. Number in parenthesis refers to manual page section number

            1. Section 2 has system calls and section 3 has library calls

            1. See `man man` for more information

        1. demo: ltrace: like strace for library calls

    1. Invocation of a syscall is architecture-specific

        1. First, specify the syscall and arguments

        1. Second, give up control to the hardware

        1. On arm64: `svc #0` gives up control

            1. Specify syscall number in `x8`

            1. Specify arguments 1-6 in `x0`, `x1`, `x2`, `x3`, `x4`, `x5`

            1. Return value will land in `x0`

        1. On x86_64: `syscall` gives up control

            1. Specify syscall number in `rax`

            1. args 1-6 in: `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`

            1. Note: this differs from normal function calling convention

                1. The `syscall` instruction clobbers `rcx`

                1.  Use `r10` instead of `rcx`

            1. Return value will land in `rax`

    1. With arguments chosen and syscall selected

        1. Give up control to the hardware

1. Hardware-assisted privilege escalation

    1. This step is done by hardware

    1. Rewind to boot:

        1. Linux installs it's syscalls into our CPU

        1. On arm64: see `__primary_switched`

            1. [Set `VBAR_EL1`](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/kernel/head.S#L477) to address of vector table

            1. Vector table defined in [entry.S](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/kernel/entry.S#L512)

        1. On x86_64: see `syscall_init()`

            1. [Set MSR_LSTAR](https://elixir.bootlin.com/linux/v6.5/source/arch/x86/kernel/cpu/common.c#L2054) to entry_SYSCALL_64` address

            1. LSTAR: Long System Target Address Register

    1. Back to the present syscall

        1. The CPU is preconfigured to correctly transfer control

        1. This makes privilege escalation safe

        1. On arm64: elevate execution level

        1. On x86_64: change to ring 0

        1. Both of these are stored in a particular register

1. Kernel code handles request

    1. Part architecture-specific and part architecture generic

    1. Execution resumes from a hardware specified register rate

        1. Higher on call stack is more generic code

        1. Mostly assembly and C macros

        1. On arm64, start in `VBAR_EL1`

            1. Hardware jumps to particular offset in [`vectors`](https://elixir.bootlin.com/linux/latest/source/arch/arm64/kernel/entry.S#L519)

            1. A function defined by macro in `entry.S` calls into `C` code
            1. The first C function is [el0t_64_sync_handler()](https://elixir.bootlin.com/linux/latest/source/arch/arm64/kernel/entry-common.c#L724)

            1. This takes us to [`el0_svc_common()`](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/kernel/syscall.c#L78)

            1. The [`invoke_syscall()`](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/kernel/syscall.c#L41) indexes into jump table of handlers

            1. This architecture-generic handler is defined by a SYSCALL_DEFINE* macro

        1. On x86_64: start at [`entry_SYSCALL_64`](https://elixir.bootlin.com/linux/v6.5/source/arch/x86/entry/entry_64.S#L91)

            1. Assembly calls into the [`do_syscall_64()`](https://elixir.bootlin.com/linux/v6.5/source/arch/x86/entry/common.c#L73) C function

            1. Using a few helper functions, index into jump table of system call handlers

            1. We have another [article](https://kdlp.underground.software/articles/syscalls_end_to_end.html)  on x86_64 syscall implementation details

            1. This architecture-generic handler is defined by a SYSCALL_DEFINE* macro

    1. Run SYSCALL_DEFINE*() handler

        1. Defined in include/linux/syscalls.h

        1. Resolve to __SYSCALL_DEFINEx(x,...

        1. Five functions generated

        1. See `__do_sys##name(...`

        1. Note: no SYSCALL_DEFINE7 and above

        1. Indicate error using the errno macros

        1. Return to assembly for exit

        1. On arm64: [`el0t_64_sync()`](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/kernel/entry.S#L571) calls `ret_to_user()`

            1. Which calls `kernel_exit 0`

            1. Restore registers, including the stack pointer

            1. [`eret`](https://elixir.bootlin.com/linux/v6.5/source/arch/arm64/kernel/entry.S#L453) gives up control to the hardware once again

        1. On x86_64: [`entry_SYSCALL_64()`] prepares to return

            1. Place the userspace return address [in `rcx`](https://elixir.bootlin.com/linux/v6.5/source/arch/x86/entry/entry_64.S#L135)

            1. `sysret` is faster than `iret`

            1. [Some conditions](https://elixir.bootlin.com/linux/v6.5/source/arch/x86/entry/entry_64.S#L123) preclude usage of `sysret`

            1. Via either instruction we give up control to the hardware

1. Hardware-assisted privilege drop

    1. Less dangerous operation than escalation

    1. Restore old register and stack

    1. Drop privileges

    1. Set RIP to userspace return address

    1. On arm64:

        1. The `svc #0` instruction saves a return address in hardware

        1. The `eret` instruction sets the program counter to this value

    1. On x86_64:

        1. [`iret`](https://www.felixcloutier.com/x86/iret:iretd:iretq) loads the return address form the stack

        1. [`sysret`](https://www.felixcloutier.com/x86/sysret) returns to `rcx`

    1. Software takes control of execution

1. Userspace program continues

    1. Always check for an error

    1. The `errno` in kernel vs userspace context

        1. Kernel functions return `-errno`

        1. C library wrappers check for error

            1. Store original error in `errno`

            1. Convert return code to `-1`

            1. example: [musl syscall return](https://elixir.bootlin.com/musl/latest/source/src/internal/syscall_ret.c#L4)

        1. See: `man 3 errno`

        1. Demo: `errno` utility from `moreutils` package

    1. The program continues execution

1. The story of a system call: Summary

    1. Linux provides a stable system call API

    1. Most programs run in user execution context ("userspace")

    1. Linux code runs in several execution contexts (all "kernelspace")

    1. Hardware plays two key roles in system calls

        1. First, raising privileges and entering kernel execution context

        1. Second, dropping privileges and entering user execution context

    1. Many syscall implementation details are architecture specific

    1. The kernel defines the main syscall handler using SYSCALL_DEFINE

        1. These macros are used to define system call implementations

    1. The C library defines wrapper functions for many syscalls

        1. These hide architecture-specific details

        1. Provide POSIX-compatible behavior by hiding Linux eccentricities

    1. Always check for an error after making a syscall
