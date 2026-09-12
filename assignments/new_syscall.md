## Add a new system call 🤫

Add a new system call to the kernel

For the purposes of grading, this assignment will be part of the "Homework Exercises" category.

#### Outcomes:

* Get comfortable with the concept of a system call from both kernelspace and userspace

    * Learn how userspace programs make system calls

    * Learn how those calls are handled by the kernel

* Understand how system calls are made in assembly language

#### What to submit:

* Patch 1 adds `kernel.patch`, which patches the kernel code to implement your system call

    * This patch adds the patch file you generated from the kernel repo *as a .patch file* to the submissions repo

    * Generate a single patch from your commit to the Linux kernel using the command `git format-patch -1` within your linux repo

    * Take the `.patch` file outputted, and put it in your named directory (e.g. `cp 0001*.patch ~/submissions/$USER/new_syscall/kernel.patch`)

    * Add *the email patch itself* as a file named `kernel.patch` to the staged git files and commit this

        * Note: there will be whitespace errors from adding the patch. It is safe to ignore them

* Patch 2 adds `Makefile` and `test.c` a program which invokes your new system call using the `syscall` function from libc, and the `patched_output.txt` file containing output of your `qemu` virtual machine

* Patch 3 adds `kdlp_syscall.S` that directly invokes the syscall from assembly, modifies `test.c` and `Makefile` to use the assembly function instead and adds the `assembly_output.txt` file containing the updated output from `qemu`

* Patch 4 adds `questions.txt` and the `original_output.txt` file containing the results of running your new init program on your unpatched kernel

* Don't forget a cover letter

* Submit your patches to `new_syscall@fall2026-uml.kdlp.underground.software`

#### Procedure:

0. Start by backing up your compiled kernel from the setup assignment

    0. Enter the `linux` repository

    0. copy `arch/riscv/boot/Image` to `~/kernel-original`

0. Add a new system call to the Linux kernel

    0. Create a new `.c` file in the `kernel` directory within the linux repo named `kdlp.c`

    0. Using the appropriately numbered `SYSCALL_DEFINE` macro, define a system call entry function for a new system call named `kdlp` (1st argument to `SYSCALL_DEFINE`) in the `kdlp.c` file that behaves as follows:

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

    0. Add a table entry to the bottom of the system call table

        * On RISC-V this is the modern shared table `scripts/syscall.tbl`

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

        * Use the same `qemu` command from [setup](/assignments/setup.md)

                qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -kernel arch/riscv/boot/Image -initrd ../rootfs.cpio -append 'panic=-1'

        * If all is well it should boot successfully

        * Congrats, you just wrote your first kernel code!

0. Make a patch out of your kernel code

    * Find all untracked files, and files you modified by looking at the output of `git status` within your linux kernel repository

    * Add them to the staging area with `git add` and then run `git commit -s` and provide a message

    * Format the resulting patch as a `.patch` file with `git format-patch -1`

        * You don't need `--rfc` or `-v1` or `--cover-letter` because this `.patch` is not getting emailed directly

    * Copy the `.patch` file into your `new_syscall` folder within your named folder in the submissions repo

    * You can make your first commit for the assignment

0. Compile the C library so you can write some more serious programs comfortably without needing to use assembly for everything

    * Prepare a sysroot folder that will hold the compiled libraries and header files in an organized fashion for the C compiler to find them:

            cd
            mkdir sysroot

    * Generate the linux kernel userspace API headers

            cd linux
            ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc) headers

    * Populate the sysroot with the generated kernel headers

            ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make INSTALL_HDR_PATH=../sysroot/usr headers_install

    * Copy the provided `libc.config` configuration file and `libc_test.c` test program from the submissions folder into your home directory

            cd
            cp ~/submissions/new_syscall/libc* .

    * Clone the v1.0.59 release of the uClibc embedded C library from from git.uclibc-ng.org

            git clone --branch=v1.0.59 https://git.uclibc-ng.org/git/uclibc-ng.git 

    * Enter the new directory for the clone

            cd uclibc-ng

    * Create a branch to make it easier to track your future work.
    You will notice a message about being in a "detached head state".
    When the `--branch` argument to git clone is supplied with a tag (e.g. vX.Y.Z), git doesn't make a branch automatically.

            git checkout -b master

    * Generate a minimal config based on the provided seed

            ARCH=riscv64 CROSS_COMPILE=riscv64-linux-gnu- make KCONFIG_ALLCONFIG=../libc.config allnoconfig

    * Compile the C library (this may take a few minutes like compiling the kernel, but shouldn't be too bad)

            ARCH=riscv64 CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc)

    * Install the compiled libraries and generated header files into the sysroot

            ARCH=riscv64 CROSS_COMPILE=riscv64-linux-gnu- make PREFIX=../sysroot install

    * Verify that the provided test program works

            cd
            riscv64-linux-gnu-gcc -fno-link-libatomic -march=rv64i -mabi=lp64 -gdwarf-5 -static-pie -fPIC --sysroot=sysroot libc_test.c -o rootfs/init
            cd rootfs && find . | cpio -co > ../rootfs.cpio && cd ..
            cd linux
            qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -kernel arch/riscv/boot/Image -initrd ../rootfs.cpio -append 'panic=-1'

    * If you are successful, you should see a message "Hello from C!" at the end of the kernel output.

0. Write a test program for your new syscall in C

    * Use the `syscall` function from libc and the macro you defined `__NR_kdlp` to invoke your new system call.

    * Check a return code of `-1` to see if there is an error.

        * Use `strerror` with `errno` to get a description of what went wrong and print an error message

        * Exit the program with a return code equal to the error number.

    * If there is not an error, use the returned length to print the appropriate amount of bytes using the `write` system call.

    * You will need to power off the kernel to exit gracefully since this program will run as pid 1

        * Feel free to start with the libc test program

        * Notice how the C library abstracts some of the details of the `reboot` system call for you and how you can use nice named constants instead of random numbers

    * Create a makefile that compiles your program using the appropriate flags

    * Rebuild the initrd using your new program as `/init` and boot into the kernel

    * Save the output similarly to how you did for `setup` to prove you got it working

    * You may now make your second commit

0. Write a dedicated helper in assembly to directly invoke the system call from the kernel

    * Create a file `kdlp_syscall.S` that contains a single function `kdlp` that sets up the arguments correctly and invokes the kernel to perform your new syscall

    * the capital `.S` extension means that the code will go through the preprocessor so you can `#include <asm/unistd.h>` to access the `#define` for your `__NR_kdlp` instead of hardcoding the literal value

    * Follow the conventions of the [ELF psABI](https://github.com/riscv-non-isa/riscv-elf-psabi-doc/releases/tag/v1.0) (processor specific application binary interface) for RISC-V to interoperate with the C code that will call your function

    * Follow the architecture calling conventions of the linux kernel for RISC-V from the [syscall manpage](https://man7.org/linux/man-pages/man2/syscall.2.html)

    * Modify `test.c` to use your `kdlp` function instead of the generic `syscall` function

        * The kernel returns the negation of the error number from the syscall if something goes wrong

        * You will need to adjust the error handling because the `errno` variable is a userspace only construct

    * To call the function that is defined in assembly, simply add an `extern`  declaration for the assembly function at the top of your file.

        * Make sure you remembered to set the `kdlp` function to have global linkage in your assembly source

    * Update `Makefile` to compile the two source files together into the program using the appropriate flags.

    * Rebuild and rerun the kernel capturing the output in `assembly_output.txt`

    * You may now make your third commit

0. Answer the following questions in a new text file at `~/submissions/$USER/new_syscall/questions.txt`

    * What did `get_task_comm` report as the name of the current task? Why?

    * Try booting your new init program with your original kernel (`~/kernel-original`). What happens?

        * What exit code does your init program return? Why?

0. Run `qemu` with your original kernel and your new init program again, capturing the output in `~/submissions/$USER/new_syscall/original_output.txt`

    * You can now make your fourth commit for the assignment

0. Extra Credit opportunity:

    * Add new code to libc to make a proper wrapper for the kdlp system call

        * Ensure that it behaves like other syscall wrappers properly interacting with `errno`

        * Don't just copy your assembly code into the libc directory and hack at the makefile to compile it in

        * I am being intentionally vague to give you an opportunity to impress

        * Figure out how to idiomatically extend the existing code to add support for a new system call

        * Hints: consult the git log for uClibc-ng; use a debugger to step through your program and find out where in libc the actual syscalls are getting made

    * Commit your changes in the uclibc-ng repository and format a patch from them

    * Copy the patch into your folder and make a fifth commit out of it like you did for the kernel patch

This page could be useful: <https://elixir.bootlin.com/linux/latest/ident/get_task_comm>

Refer to the [Linux kernel documentation about adding system calls](https://www.kernel.org/doc/html/latest/process/adding-syscalls.html) for further guidance.

[Frequently Asked Questions](/faq.md)
