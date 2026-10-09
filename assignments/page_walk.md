## Page Walk 🚶

Add an entry to /proc that walks the page table and exposes the physical mappings underlying virtual memory addresses

For the purposes of grading, this assignment will be part of the "Programming Assignments" category.

#### Outcomes:

* Understand how the page table maps virtual memory addresses to physical memory addresses

#### What to submit:

* Patch 1 adds the module code and makefile to `submissions/$USER/page_walk/`

* Patch 2 adds your testing program and appropriate Makefile which invokes your new system call and prints the required results accordingly

* Patch 3 adds testing_output.txt and questions.txt containing output from running your program in the VM and some commentary

* Don't forget a cover letter

* Submit your patches to page_walk@fall2026-uml.kdlp.underground.software

#### Procedure:

* From now on when building the kernel, use a config that has the MMU enabled

    * Create a new config using `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make menuconfig`

        * Navigate to the `MMU-based Paged Memory Management Support` option with the up/down arrows and press `y` to enable the MMU

        * This also disables the option `Build a kernel that runs in machine mode`

    * You must remove the `-bios none` flag when starting a new `qemu` virtual machine

        * When the kernel is no longer running in machine mode, something must take responsibility for the startup process that must occur in machine mode

        * QEMU provides OpenSBI to do this by default, and we were suppressing this behavior with `-bios none`

* Implement a module to create `/proc/page_walk`

    * The file will represent a window into the entire virtual address space similar to the existing `/proc/self/mem` or `/dev/mem` files

        * Instead of accessing the contents of memory at a given virtual address, however, this file will provide metadata a particular virtual address revealing the page tables underlying the mapping from that virtual address to the physical address where the data lives

        * A particular location can be selected using the virtual address interpreted as an offset for `pread`

        * Reading from the file will generate a report on the page tables underlying the mapping of whichever byte corresponds to the file offset interpreted as a virtual address within the current process

    * The skeleton of your module that creates and removes the file in `/proc` will be similar to that of `first_module`, however:

        * You will not be able to use the same `struct seq_file` based technique from `/proc/cmdline`, with a "show" function

        * instead the module will need a `struct proc_ops` structure with the `proc_read` member pointing to your function that directly implements the custom behavior of the read system call

        * You will need the more nuanced `proc_create` API as seen in the code for `/proc/config.gz` so you can pass your operations struct

    * The read implementation for the page walk file expects always to be given a pointer to an array 6 `unsigned long`s as the user buffer and an appropriately large count

        * It will interpret the current file offset as a virtual address and walk the page table to try and locate the corresponding page table entry (PTE)

            * Starting with the value of the supervisor address translation and protection (SATP) register, and continuing with each PTE encountered,
            the system call records the data it processes

                * Each value (the SATP and the subsequent PTEs) are copied into a local array of `unsigned long`s

            * When the walk terminates (either by reaching an invalid mapping or locating the terminal page table entry), the system call copies the data from
            the local array of `unsigned long`s to the provided user array

                * If that copy fails, the system call returns `-EFAULT`

                * Otherwise, it returns the size in bytes of the data copied to the user array

        * The [RISC-V ISA Manual](https://github.com/riscv/riscv-isa-manual/releases) contains important reference material you will need to replicate how paging works on RISC-V

            * Section 4.3.2 "Virtual Address Translation Process" describes what the MMU hardware in the CPU does to perform a page lookup

                * These instructions are describing in detail the actions taken for an MMU operating within the Sv32 virtual memory system

                * Sv32 is only used on 32 bit RISC-V, so our 64 bit machine will not be using it, but the other addressing modes are explained only in how they differ from Sv32

                * Read the `satp` register yourself to determine which virtual addressing mode your virtual machine is using and act occording to the specification for that mode

        * You will probably need the following new kernel functions and macros

            * `phys_to_virt`

            * `csr_read`

            * `CSR_SATP`

* Create a new C program to test your implementation of `/proc/page_walk`

    * Your program should exercise your code using `pread` with offsets based on pointers from different regions of the virtual address space

        * Your program should explain what it is testing, then print the output that it received

        * Your program should extract the physical page number (PPN), and the dirty, accessed, global, user, execute, write, read, and valid flags

    * Access your file at least five times where each call yields a unique combination of flags

        * You shouldn't have a hard time finding five different flag combinations by passing pointers to different kinds of data that exist in the address space of a normal C program

            * Think different types of symbols, objects with different storage durations, and how a program can change its own address space directly or indirectly

            * Extra credit is available if you can find more than five unique combinations of flags

* Document your findings

    * Run your program in the VM and capture the output for your third commit

    * Answer the following questions in `questions.txt`

        * How did you find new combinations of flags?

        * For each of the five (or more) flag combinations you found, answer: Why does the pointer you passed refer to memory in a page with the particular flags you found?

[Frequently Asked Questions](/faq.md)
