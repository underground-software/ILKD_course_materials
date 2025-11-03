## Page Walk 🚶

Add a new system call that walks the page table and exposes the physical mappings underlying virtual memory addresses

For the purposes of grading, this assignment will be part of the "Homework Exercises" category.

#### Outcomes:

* Understand how the page table maps virtual memory addresses to physical memory addresses

#### What to submit:

* Patch 1 adds `syscall.patch`, which patches the v6.16 kernel code to implement a `riscv_pagetable_walk` system call

    * This patch adds the patch file you generated from the kernel repo *as a .patch file* to the submissions repo

        * Note that this means that if you try to apply this patch locally it will show as having whitespace errors

        * It is safe to ignore them, and the course dashboard will as well (i.e. you should not see a bogus "issues detected in submission" message this time)

    * Generate a single patch from your commit to the Linux kernel using the command `git format-patch -1` within your linux repo

    * Take the `.patch` file outputted, and put it in your named directory (e.g. `cp 0001*.patch ~/submissions/$USER/page_walk/syscall.patch`)

    * Add *the email patch itself* as a file named `syscall.patch` to the staged git files and commit this

* Patch 2 adds your testing program and appropriate Makefile which invokes your new system call and prints the required results accordingly

* Patch 3 adds testing_output.txt and questions.txt containing output from running your program in the VM and some commentary

* Don't forget a cover letter

* Submit your patches to page_walk@fall2025-uml.kdlp.underground.software

#### Procedure:

* From now on when building the kernel, use a config that has the MMU enabled

    * Create a new config using `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make menuconfig`

        * Navigate to the `MMU-based Paged Memory Management Support` option with the up/down arrows and press `y` to enable the MMU

        * This also disables the option `Build a kernel that runs in machine mode`

    * You must remove the `-bios none` flag when starting a new `qemu` virtual machine

        * When the kernel is no longer running in machine mode, something must take responsibility for the startup process that must occur in machine mode

        * QEMU provides OpenSBI to do this by default, and we were suppressing this behavior with `-bios none`

* Create a new branch in the `linux` repo with no changes from previous assignments

    * i.e. `git checkout -b pagetable_walk_syscall v6.16`

* Create a new system call, `riscv_pagetable_walk`

    * The definition of the system call and appropriately numbered `SYSCALL_DEFINE` macro should be written in a new file `arch/riscv/mm/riscv_pagetable_walk.c`

    * This new system call takes two arguments: a `void *` pointer that is the virtual memory address to locate in the page table and a `unsigned long __user *`
    which is an array of 6 `unsigned long`s

        * This system call will walk the page table to try and locate a given pointer's corresponding page table entry (PTE)

            * Starting with the value of the supervisor address translation and protection (SATP) register, and continuing with each PTE encountered,
            the system call records the data it processes

                * Each value (the SATP and the subsequent PTEs) are copied into a local array of `unsigned long`s

            * When the walk terminates (either by reaching an invalid mapping or locating the terminal page table entry), the system call copies the data from
            the local array of `unsigned long`s to the provided user array

                * If that copy fails, the system call returns `-EFAULT`

                * Otherwise, it returns the number of `unsigned long`s copied to the user array

        * The [RISC-V privileged ISA](https://github.com/riscv/riscv-isa-manual/releases) contains all of the reference you should need

            * Section 12.3.2 describes the virtual address translation process

                * These instructions assume you are using the Sv32 virtual memory system, read the `satp` register yourself to determine which
                virtual addressing mode your virtual machine is using and find the specification for that mode

        * You will probably need the following new kernel functions and macros

            * `phys_to_virt`

            * `csr_read`

            * `CSR_SATP`

    * Because of the architecture-specific nature of this pagetable walk, this system call should be available only on RISC-V

        * The process for adding an architecture-specific system call is different from the process you followed in the [new_syscall](new_syscall.md) assignment

    * Add `riscv_pagetable_walk.o` to `obj-$(CONFIG_MMU)` in `arch/riscv/mm/Makefile`

    * Add an `asmlinkage` declaration of `riscv_pagetable_walk` in `arch/riscv/include/asm/syscall.h`

    * Add a table entry to `scripts/syscall.tbl`

        * Each architecture is allowed up to 16 [architecture specific syscalls](https://elixir.bootlin.com/linux/v6.16/source/scripts/syscall.tbl#L291)

        * Add your entry at the appropriate location according to its number

        * Choose an appropriate number given the two existing riscv specific syscalls and the acceptable range of numbers

        * make sure your set the architecture to `riscv`

* Create a new C program to test your implementation of `riscv_pagetable_walk`

    * Your program should exercise your new system call on pointers to different types of data

        * Your program should explain what it is testing, then print the output that it received

        * Your program should extract the physical page number (PPN), and the dirty, accessed, global, user, execute, write, read, and valid flags

    * Call your system call at least five times where each call yields a unique combination of flags

        * You shouldn't have a hard time finding five different flag combinations by passing pointers to different kinds of data that exist in the address space of a normal C program

            * Extra credit is available if you can find more than five unique combinations of flags

* Document your findings

    * Run your program in the VM and capture the output for your third commit

    * Answer the following questions in `questions.txt`

        * How did you find new combinations of flags?

        * For each of the five (or more) flag combinations you found, answer: Why does the pointer you passed refer to memory in a page with the particular flags you found?

[Frequently Asked Questions](/faq.md)
