## Page Walk 🚶

Add a new system call that walks the page table and translates virtual memory addresses to
physical addresses

#### Outcomes:

* Understand how the page table maps virtual memory addresses to physical memory addresses

* Gain experience writing C code with no C runtime and no C library

#### What to submit:

* Patch 1 adds `syscall.patch`, which patches the v6.13 kernel code to implement a `riscv_pagetable_walk` system call

    * This patch adds the patch file you generated from the kernel repo *as a .patch file* to the submissions repo

        * Note that this is the **only time** it is acceptable to submit a patch that contains whitespace errors as this is an artifact of committing an email code patch

    * Generate a single patch from your commit to the Linux kernel using the command `git format-patch -1` within your linux repo

    * Take the `.patch` file outputted, and put it in your named directory (e.g. `cp 0001*.patch ~/submissions/$USER/pagewalk_syscall/syscall.patch`)

    * Add *the email patch itself* as a file named `syscall.patch` to the staged git files and commit this

* Patch 2 adds your new init program which invokes your new system call and prints the required results accordingly

* Don't forget a cover letter

* Submit your patches to page_walk@spring2025-utsa.kdlp.underground.software

#### Procedure:

* From now on when building the kernel, use a config that has the MMU enabled

    * Create a new config using `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make menuconfig`

        * Navigate to the `MMU-based Paged Memory Management Support` option with the up/down arrows and press `y` to enable the MMU

        * This also disables the option `Build a kernel that runs in machine mode`

    * You must remove the `-bios none` flag when starting a new `qemu` virtual machine

        * When the kernel is no longer running in machine mode, something must take responsibility for the startup process that must occur in machine mode

        * Qemu provides OpenSBI to do this by default, and we were suppressing this behavior with `-bios none`

* Create a new branch in the `linux` repo with no changes from previous assignments

    * i.e. `git checkout -b pagetable_walk_syscall v6.13`

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

    * Because of the architecture-specific nature of this pagetable walk, this system call should be avialable only on RISC-V

        * The process for adding an architecture-specific system call is different from the process you followed in the [new_syscall](new_syscall.md) assignment

    * Add `riscv_pagetable_walk.o` to `obj-$(CONFIG_MMU)` in `arch/riscv/mm/Makefile`

    * Add an `asmlinkage` declaration of `riscv_pagetable_walk` in `arch/riscv/include/asm/syscall.h`

    * Allocate an architecture-specific system call number

        * The kernel provides a `__NR_arch_specific_syscall` macro

            * See the [definition and a brief explanation here](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/asm-generic/unistd.h#L628)

        * Define `__NR_riscv_pagetable_walk` in `tools/arch/riscv/include/uapi/asm/unistd.h`

            * See the definition of `__NR_riscv_flush_icache` in this file for guidance

            * Assign your system call the number `__NR_arch_specific_syscall + 13`

                * `__NR_arch_specific_syscall + 14` and `__NR_arch_specific_syscall + 15` are already taken by other RISC-V-specific sytem calls

                * There is a [limit of 16 architecture specific syscalls](https://elixir.bootlin.com/linux/v6.13/source/include/uapi/asm-generic/unistd.h#L625)

                * Use `+ 13` which is available in riscv kernel v6.13

    * Add a table entry to `scripts/syscall.tbl`

        * Add your entry at the apropriate location according to its number

            * make sure your set the architecture to `riscv`

* Create a new init program to test your implementation of `riscv_pagetable_walk`

    * Create a new c file to act as your init program

    * When compiling your program, use `riscv64-linux-gnu-gcc -nostdlib walk-init.c -march=rv64i -mabi=lp64 -o rootfs/init`

    * Even though we now have an MMU, we still don't have a C runtime

        * Instead of writing a `main` function, define a function `void _start(void)`

            * This will be your init program's entrypoint

    * We also don't have a standard library, so you will have to write some inline assembly or link with a separate `.s` file in order to create a `syscall(...)` wrapper function

    * Your program should exercise your new system call on pointers to different types of data

        * Your program should explain what it is testing, then print the output that it received

        * Your program should extract the physical page number (PPN), and the dirty, accessed, global, user, execute, write, read, and valid flags

    * Call your system call at least five times where each call yields a unique combination of flags

        * You shouldn't have a hard time finding five different flag combinations by passing pointers to different kinds of data that exist in the address space of a normal C program

            * Extra credit is available if you can find more than five unique combinations of flags

        * In your cover letter when discussing your approach to completing the assignment:
            
            * Document how you went about finding new combinations of flags

            * Explain why each type of the data is market by its particular flags

    * Call the `reboot` system call

        * Use the same args as used in previous assignments to shut down: `0xfee1dead, 0x28121969, 0x4321fedc`
