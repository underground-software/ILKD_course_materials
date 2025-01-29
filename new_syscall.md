## Add a new system call 🤫

Add a new system call to the kernel

#### Outcomes:

* Get comfortable with the concept of a system call from both kernelspace and userspace

    * Learn how userspace programs make system calls

    * Learn how those calls are handled by the kernel

* Understand how system calls are made in assembly language

#### What to submit:

* Patch 1 adds `syscall.patch`, which patches the v6.13 kernel code to implement your system call

    * This patch adds the patch file you generated from the kernel repo *as a .patch file* to the submissions repo

        * Note that this is the **only time** it is acceptable to submit a patch that contains whitespace errors as this is an artifact of committing an email code patch

    * Generate a single patch from your commit to the Linux kernel using the command `git format-patch -1` within your linux repo

    * Take the `.patch` file outputted, and put it in your named directory (e.g. `cp 0001*.patch ~/submissions/$USER/new_syscall/syscall.patch`)

    * Add *the email patch itself* as a file named `syscall.patch` to the staged git files and commit this

* Patch 2 adds `syscall.s`, an assembly program which invokes your new system call, and the `patched_output.txt` file containing output of your `qemu` virtual machine

* Patch 3 adds `questions.txt` and the `vanilla_output.txt` file containing the results of running your new init program on your unpatched kernel

* Don't forget a cover letter

* Make sure to review the [policies and procedures](/procedures.md)

* Submit your patches to new_syscall@spring2025-utsa.kdlp.underground.software

#### Procedure:

0. Start by backing up your compiled kernel from setup

    0. Enter the `linux` repository

    0. copy `arch/riscv/boot/Image` to `~/kernel-v6.13`

0. Add a new system call to the Linux kernel

    0. Create a new `.c` file in the `kernel` directory within the linux repo named `kdlp.c`

    0. Using the appropriately numbered `SYSCALL_DEFINE` macro, define a system call entry function for a new `kdlp` system call in the `kdlp.c` file that behaves as follows:

        * Takes two arguments, an argument of type `char __user *` to specify the buffer and an argument of type `size_t` to specify the size of the buffer

        * Formats a message into a local buffer that includes at least the student's name and the name of the current task that is running on the CPU, obtained via the `get_task_comm` macro

        * Determines the amount of data to send: equal to the lesser of the size of the kernel message and how much space the user provided (see `min` macro)

        * Copies that many bytes from the local message buffer into the user pointer (see the `copy_to_user` function)

        * Returns a negative error code of `-EFAULT` (errno 14, bad address) if the copy is not successful

        * Otherwise, returns the amount of data that was copied

    0. The system call implementation should take care to prevent any possibility of a buffer overflow

    0. Modify the makefile for the kernel subdirectory in `kernel/Makefile` to add `kdlp.o` to the list of objects that are always compiled (add it to the end of the `obj-y` variable)

    0. Add an `asmlinkage` declaration of `sys_kdlp` to `include/linux/syscalls.h` (take inspiration from the numerous examples of other system calls in that file)

    0. Add an entry to the system call numbering header

        * On RISC-V the list comes from the modern shared file `include/uapi/asm-generic/unistd.h`

            * Find where `__NR_syscalls` is defined and increment the value

            * Just above that line, add a `#define` for `__NR_kdlp` with the next system call number that is free

            * Add an invocation of the `__SYSCALL` macro with the `__NR_kdlp` number and and the `sys_kdlp` entry point

            * Take inspiration from the other nearby system calls

    0. Add a table entry to the bottom of the system call table at `scripts/syscall.tbl`

        * Take inspiration from the other nearby system calls

    0. Before compiling the entire kernel, you will find it helpful to compile your new kdlp.c file in isolation using `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make kernel/kdlp.o`

        * If there are any errors, fix them and re-run the above command until the compilation succeeds

    0. Run `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make clean` in your cloned `linux` repo

        * This will remove any artifacts from previous compilations

        * You must do this before modifying the `localversion`

    0. Update the `localversion` file to create a distinct release string so you can tell this kernel that has your system call apart from any other kernels you have compiled

    0. Compile your new kernel with `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc)`

    0. If the command finishes, make sure it was successful: `echo $?` should output `0`

        * If it does not, re-run `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make` without `-j $(nproc)` to get a better error message

        * Fix whatever issue you see, and re-run `ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc)`

        * Repeat this process of checking the result and fixing any errors until it compiles successfully

    0. Boot your new kernel with your the old init program from setup

        * Use the same `qemu` command from [setup](/setup)

        * If all is well it should boot successfully

        * Congrats, you just wrote your first kernel code!

0. Make a patch out of your kernel code

    * Find all untracked files, and files you modified by looking at the output of `git status` within your linux kernel repository

    * Add them to the staging area with `git add` and then run `git commit -s` and provide a message

    * Format the resulting patch as a `.patch` file with `git format-patch -1`

        * You don't need `--rfc` or `-v1` or `--cover-letter` because this `.patch` is not getting emailed directly

    * Copy the `.patch` file into your `new_syscall` folder within your named folder in the submissions repo

    * You can make your first commit for the assignment

0. Write an assembly language program named `syscall.s` that invokes your new system call

    * Use `init.s` from [setup](/setup.md) as a starting point

    * Invoke your new system call and print the message you received from the kernel using the `write(2)` system call and keep the code that shuts down the system upon completion

    * Explicitly invoke the system calls by following the conventions of the linux system call ABI (application binary interface) for RISC-V:

        * Arguments go into specific registers in order (`a0`, `a1`, `a2`, `a3`, `a4`, `a5`)

        * The system call number goes into `a7`

        * The system call is invoked using the `ecall` instruction

        * The return value is in `a0` and is either a non-negative return value, or a negative error number

    * Check for any errors and print a simple error message if any occur

        * The kernel will return a negative number on errors

        * After printing, invoke the exit system call and pass a positive version of the error number

    * Refer to the instructions and files provided in [setup](/setup.md)

    * Compile `syscall.s` using the `gcc` command from [setup](/setup.md)

        * Make sure it builds correctly

        * This will be your new init program

    * Rebuild your initrd using your new init program using the `cpio` commands from [setup](/setup.md)

    * Boot a your new kernel using your new init program in `qemu` with the same commands 

    * If all is well, you should see your message in the `qemu` output and the virtual machine will halt gracefully

    * Run `qemu` again, redirecting the output to `~/submissions/$USER/new_syscall/patched_output.txt`

    * You can now make your second commit for the assignment

0. Answer the following questions in a new text file at `~/submissions/$USER/new_syscall/questions.txt`

    * What did `get_task_comm` report as the name of the current task? Why?

    * Try booting your new init program with your original kernel (`~/kernel-v6.13`). What happens?

        * What exit code does your init program return? Why?

0. Run `qemu` with your original kernel and your new init program again, redirecting the output to `~/submissions/$USER/new_syscall/vanilla_output.txt` 

    * You can now make your third commit for the assignment

This page could be useful: This page should be useful: https://elixir.bootlin.com/linux/latest/ident/get_task_comm

Refer to the [Linux kernel documentation about adding system calls](https://www.kernel.org/doc/html/latest/process/adding-syscalls.html) for further guidance.

[Frequently Asked Questions](/faq/faq.md)
