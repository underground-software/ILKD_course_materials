# interrupts

1. Objective: Understand how Linux processes interrupts

1. What interrupts are & Linux overview

    1. When you press a button on your keyboard, what happens?

    1. Why is the computer able to react right away?

    1. Motivation: why not just poll for new input?

    1. Definition: An interrupt is a signal to the CPU to stop what it's doing and take care of urgent business

1. Most important thing?

    1. Speed

    1. Also important: Atomicity

1. General idea of interrupts

    1. Something needs to be done urgently

        1. Otherwise we would use polling

    1. First, we take care of the most urgent parts

        1. This is the top-half

        1. AKA: Interrupt Service Routine (ISR)

        1. Schedule less urgent action

    1. Second, we take care of the less urgent parts

        1. This is the bottom-half

        1. Multiple types of bottom halves

1. May be hardware or software generated

    1. Hardware or software interrupts

1. Terminology

    1. Trap can refer to
        
        1. any interrupt

        1. any software interrupt

        1. any synchronous software interrupt

        1. any interrupt caused by "trap" instruction

    1. May even refer to debugger breakpoints

    1. Synchronous software interrupts are generally referred to as exceptions

    1. arm64

        1. exception: any type of interrupt

        1. interrupt: hardware interrupt

        1. abort: prefetch or data abort (page fault)
            
            1. like an x86 exception

        1. reset: reinit the processor state

        1. Some instructions are exception-generating instructions

    1. x86_64

        1. interrupts: hardware interrupts

        1. exceptions: software interrupts

            1. fault: return to save instruction

            1. trap: return to next instruction

            1. aborts: don't return to program

1. Details on kernel execution context

    1. previously: kernelspace and userspace context

    1. now: further deliniation on kernel context

        1. process context: kernel running attached to a process

            1. `current` refers to running process' `struct task_struct`

        1. interrupt context: kernel running NOT attached to a process

            1. No sleeping

            1. Interrupts disabled

            1. Interrupt Service Routine (ISR) runs here

        1. kernel-thread context: kernel running NOT attached to a process

            1. softirq: interrupts enabled but same context

            1. tasklets: based on softirqs

                1. can be scheduled to run later

            1. workqueue: process context
                
                1. can sleep

                1. good for more extensive processing

    1. [include/linux/interrupt.h](https://elixir.bootlin.com/linux/v6.5/source/include/linux/interrupt.h)
