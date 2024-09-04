# System Calls
When we talk about a system call (for brevity's sake, we will refer to them primarily as **syscalls**), the idea that may first come to mind is of a set of highly specialized utilities that enable the systems programmer to perform arcane work that the average software engineer would not need to concern themselves with. 

This is true, in a sense--most software does not bother to directly use them, but all[^1] code that does anything useful invokes syscalls, though usually via library functions. For example, when we use `printf` to print a string to `stdout`,  the `write` syscall is being used. When we use `fopen`, we invoke the `open` syscall, and in fact any time a program exits we use the `exit` syscall. They are omnipresent, and it behooves us to gain a better understanding of how exactly they function, such that you can feel comfortable writing code that uses syscalls directly, without the mediation of a library.
## Part I: Necessary Background
Broadly defined, a **syscall** is:
1. A userspace request for kernel action,
1. The mechanism by which we escalate privileges.[^2]

In our pursuit of elaborating the role syscalls play in the Linux kernel, there are several key concepts we must address first. Many of these we will return to later in our course of study, but for now we will minimize our digressions to what is absolutely necessary.
### Some context on contexts
An execution context, put simply, is a CPU register state. The idea of a context switch should be very familiar: when we run a program, it has a stack pointer, an instruction pointer, and other program-dependent values for the registers. It is evident that when we run a program, our computer continues running other programs, and we don't lose our place in one process when another process gets its turn on the CPU--context switches are how this change is handled.

We can demonstrate this idea with a simple example program (we urge the reader to try and figure out what this program will print/what code is unreachable before looking at the output below the program body):
```c
#include <setjmp.h>
#include <stdio.h>

jmp_buf buf;

void bar(void){
    puts("Bar enter");
    longjmp(buf, 0xDEADBEEF);
    puts("Bar exit");
}

void foo(void){
    puts("Foo enter");
    bar();
    puts("Foo exit");
}

int main(void)
{
    switch(setjmp(buf)){
    case 0:
        puts("Zero branch enter");
        foo();
        puts("Zero branch exit");
        break;
    case 0xDEADBEEF:
        puts("0xDEADBEEF branch");
        break;
    default:
        puts("Default branch");
    }
    return 0;
}
```
If you're not familiar with the `setjmp` and `longjmp` functions, what's critical to understand here is that `setjmp` will store the current state of the registers (and other components of the abstract machine) to the `buf`  struct and return 0, and `longjmp` will load the state of the registers back from `buf`. When the register state is restored from `buf`, the instruction pointer (on arm64, this is called the program counter) is among those that will be updated, and we'll end up returning to the line where `setjmp` was executed. To avoid causing an infinite loop, the second argument to `longjmp` will be the new return value, and we'll continue down a different path than the one we went down first. To learn more, consult `man 3 setjmp` and `man 3 longjmp`.

With this in mind, the output of the program should now make sense:
```
Program returned: 0
Program stdout:
Zero branch enter
Foo enter
Bar enter
0xDEADBEEF branch
```
Another example that helps us outline the concept of a context is threading. When we spawn a new thread, a new context is created: which is to say, we're given a new instruction pointer, a new stack pointer, and the remaining registers will be stored and then reloaded when we switch to and from the thread.

![Visual demonstration of what is shared and what is not with threads](https://www.w3.org/People/Frystyk/thesis/MultiStackThread.gif)
In the same sense, kernelspace is a separate context from userspace, but it is worth mentioning that the kernel doesn't just have one context, just as our own programs in userspace may not have a single context. The kernel has a context for running code on its own behalf, called **kernel interrupt context**, which comes with a host of restrictions about sleeping, blocking, and otherwise interfering with the scheduler. We'll look more closely at this when we get to discussing interrupts. For now, we'll be primarily examining **kernel process context**, which is where the kernel executes code on behalf of a userspace process via a syscall (or an exception). When we use a syscall and ask the kernel to execute some code on our program's behalf, a context switch takes place; the registers from our program are stashed, we enter the kernel's context, some work is performed, and then our program's registers are restored when we return to userspace.

The primary difference between kernelspace and the userspace context which we're used to is privilege level. We won't deal with the specifics of rings just yet, but at this time it is enough to say that when the kernel executes code, the CPU's capabilities are unrestricted, and we will receive no hand-holding to prevent us from fantastically screwing up our computer. It is thus important to keep in mind that a context switch to/from the kernel is accompanied by a raising/dropping of privileges.

### Staying current
Processes have more to keep track of than just the state of their registers. The general concept here is of a [Process Control Block (PCB)](https://en.wikipedia.org/wiki/Process_control_block), which is where operating systems store information about processes. Linux's PCB is `struct task_struct`, and you can find its definition [here](https://elixir.bootlin.com/linux/latest/source/include/linux/sched.h#L748). The `task_struct` contains hundreds of fields, storing information like process id (henceforth PID), threadgroup id (henceforth TGID), a pointer to the parent `task_struct`, priority, open files and other resources, and much, much more. For each PID, there exists one and only one associated `task_struct` that contains it. To obtain the task struct of the process in the current execution context, we use the `current` macro, defined [here](https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/current.h) for arm64 and [here](https://elixir.bootlin.com/linux/latest/source/arch/x86/include/asm/current.h) for x86. The process by which `current` looks up the `task_struct` by pid is not important at this time, but suffice to say that `task_struct` is multiply-linked to make this search fairly quick.

It is important to keep in mind that what we call a **TID** in userspace is a **PID** to the kernel, and what the kernel calls a **TGID** is what we've henceforth known in userspace as a **PID**[^3].

From [`man 2 getpid`](https://man7.org/linux/man-pages/man2/getpid.2.html):

> From a kernel perspective, the PID (which is shared by all of the threads in a multithreaded process) is sometimes also known as the thread group ID (TGID).  This contrasts with the kernel thread ID (TID), which is unique for each thread.  For further details, see [`gettid(2)`](https://man7.org/linux/man-pages/man2/gettid.2.html) and the discussion of the **CLONE_THREAD** flag in [`clone(2)`](https://man7.org/linux/man-pages/man2/clone.2.html).

The reason for this distinction is historical--before the 2.6 version of the Linux kernel, the scheduler was concerned only with processes, and threads were second-class citizens. Thread-like behavior was made possible with `clone`, which would duplicate a process but keep the address space shared. This still created too much independence to be desireable--for example, the resultant processes could not share signals. The [NPTL](https://en.wikipedia.org/wiki/Native_POSIX_Thread_Library) brought us the threading landscape we have today, and its integration in to the GNU C library brought modern locking tools, as well as the concept of a TID, which subdivides a userspace PID. The concept of a TGID in the kernel then exists to re-unify the userspace TIDs, but in userspace that unification is referred to as a PID.

## Part II: What can Syscalls do for You?

Todo



[^1]: Though it's more of a curiosity than anything, [this program](evilasmbullshitfixmeputthelinkherelater) will compute whether the input is prime and "return" its result without a single syscall. The way it is able to do this, though it is architecture-dependent, is by crashing in one of two different ways--`SIGSEGV` for a prime, `SIGILL` for a compound--no syscalls necessary, though we encourage the reader to to verify this for themselves with `strace`.

[^2]: For example, this program is guaranteed to crash _when run in userspace:_
    ```
    .text
    .globl _start
    _start:
    msr VBAR_EL1, x0
    ```
    This is due to the fact that reading from model specific registers (`msr`) is a privileged activity.
[^3]: See `gettid` and `getpid` in [`kernel/sys.c`](https://elixir.bootlin.com/linux/latest/source/kernel/sys.c).
    The `getpid` callstack is approximately:
       1. `task_tgid_vnr()` (in `include/linux/pid.h`)
       1. Take namespace into account
       1. Do some locking
       1. `task_pid_nr() { ... tsk->pid ... }`
