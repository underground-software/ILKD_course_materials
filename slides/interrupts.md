# interrupts

---

# Objective: Understand how Linux processes interrupts

---

# When you press a button on your keyboard, what happens?

Why is the computer able to react right away?

---

# Motivation

Why not just poll for new input?

---

# Definition

An interrupt is a signal to the CPU to stop what
it's doing and take care of urgent business

---

# Most important thing

1. Speed

1. Also important: Atomicity

---

# General idea of interrupts

Something needs to be done urgently

1. Otherwise we would use polling

---

# General idea of interrupts

First, we take care of the most urgent parts

1. This is the top-half

1. Schedule less urgent action

---

# General idea of interrupts

Second, we take care of the less urgent parts

1. This is the bottom-half

---

# demo

Take a look at `/proc/interrupts`

---

# Two main types

1. Hardware interrupts

1. Software interrupts

---

# Terminology

Trap can refer to
    
1. any interrupt

1. any software interrupt

1. any synchronous software interrupt

1. any interrupt caused by "trap" instruction

May even refer to debugger breakpoints

---

# Exceptions

Synchronous software interrupts are generally referred to as exceptions

"Exceptional" CPU conditions

---

# arm64 terminology

1. exception: any type of interrupt

1. interrupt: hardware interrupt

1. abort: prefetch or data abort (page fault)
    
    1. like an x86 exception

1. reset: reinit the processor state

1. Some instructions are exception-generating instructions

---

# x86_64 terminology

interrupts: hardware interrupts

exceptions: software interrupts

1. fault: return to save instruction

1. trap: return to next instruction

1. aborts: don't return to program

---

# demo

Tracing interrupts with `bpftrace`

---

# Kernel execution context

Previously:

1. userspace context

1. kernelspace context

---

# Kernel execution context

process context: kernel running attached to a process

1. `current` refers to running process' `struct task_struct`

---

# Kernel execution context

interrupt context: kernel running NOT attached to a process

1. No sleeping

---

# Top Half: Interrupt Service Routine (ISR)

1. Runs in interrupt context

1. No sleeping/blocking

1. Interrupts disabled

---

# Bottom half: softirq

1. Runs in interrupt context

1. Starts immediately after the ISR

1. Interrupts enabled

---

# Bottom half: tasklet

1. Runs in interrupt context

1. Based on softirq, but can be deferred

1. Interrupts enabled

---

# Bottom half: workqueue

1. Runs in process context

1. Can sleep

1. For more extensive processing

---

# demo

Look at `ksoftirqd` in `ps`

---

# demo

A look at [include/linux/interrupt.h](https://elixir.bootlin.com/linux/v6.5/source/include/linux/interrupt.h)

---

# demo

A simple example module

---

# demo

Use `bpftrace` to see the kernel stack and dive into the code

---

# End
