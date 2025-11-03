## Your first kernel module 🍿

You will specify, implement, and test a simple kernel module that creates a `kdlp` entry in the Linux kernel's [`/proc`](https://docs.kernel.org/filesystems/proc.html) filesystem with a basic read only interface that returns a unchanging message which includes the student's name.

For the purposes of grading, this assignment will be part of the "Homework Exercises" category.

#### Outcomes:

* Get familiar with the internals of Linux's [`/proc`](https://docs.kernel.org/filesystems/proc.html) filesystem

* Learn how to specify the behavior of a kernel module with a guided example

* Get familiar with software testing, how to write good tests, and how to develop code using tests as a guide

* Write your first kernel module!

#### What to submit:

* Patch 1 adds `$USER/first_module/specification.txt`, a filled out copy of `spec.txt`

* Patch 2 adds `$USER/first_module/tests/test.c`, your original testing program, and `$USER/first_module/tests/Makefile`, which builds your testing program as `test` via the default and `test` targets

* Patch 3 adds `$USER/first_module/kdlp.c` and a new makefile to build `$USER/first_module/kdlp.ko` via the default and `build` targets

* Patch 4 adds `$USER/first_module/testing_output.txt` which demonstrates the working submission

* Don't forget a cover letter

* Submit your patches to first_module@fall2025-uml.kdlp.underground.software

#### Procedure:

0. Rebuild the kernel with support for loadable modules and the `/proc` filesystem

    * Copy `first_module/module.config` into `~` your home directory

    * Recreate the kernel `.config` file using the `allnoconfig` command from the setup assignment, but substituting in `../module.config` instead of `../init.config`

    * Rebuild the kernel image using the make command with the `Image` target

    * Build the module dependency and symbol information by using the same command as for the image, but replacing `Image` with `modules`

0. Write your specification for the module

    * Create your `$USER/first_module` directory

    * Copy `first_module/spec.txt` into your personal folder for the assignment

    * Edit your copy of `spec.txt` and fill in the blanks to fully specify the expected behavior of your module.

0. Write a testing program and makefile that exercises a module implementing the spec

    * Create a `tests/` directory in `$USER/first_module`

    * Your test program must be named `test.c` and placed in `$USER/first_module/tests/`

    * Create a makefile in your `$USER/first_module/tests/` directory that builds an executable named `test` as the default target and `test` target

    * The makefile's `clean` target must remove the test build artifacts and should return success even when no artifacts exist to be deleted

    * The testing program will performing operations on the file that the module will create in `/proc`

    * Your test program must verify the expected behavior from the specification against the actual functionality of the file in `/proc`

    * The standard output from your testing program must comply with the Test Anything Protocol version 14. See <https://testanything.org/> for more info.

    * Be sure to write tests for all the behavior (existence of the file, file permissions, behavior of reading, writing, seeking, etc.)

    * Make sure to also test error conditions to make sure strange user input to your module won't crash the system, e.g. a null or invalid pointer

    * You should have at least 10 distinct tests that the module can pass or fail

        * When run, the program should provide information about which tests succeeded or failed

        * If a test failed, include in the program output how the behavior differed from what was expected

    * At this point, you have nothing to test

        * In order to validate your testing program, temporarily change the hard-coded path of the file it opens

        * Instead of opening a file in `/proc`, you can have it open a regular file whose contents/owner/permissions you can change manually to see which tests pass or fail

        * Don't forget to change this back before committing your code (or write your program / makefile in such a way that what file is used can be overridden from the default in `/proc` for testing)

0. Implement the module

    * The module source file must be named `kdlp.c`

    * The file must be accessible at `/proc/kdlp`

    * Create a makefile in your `$USER/first_module/` directory to build/clean and load/unload your module

        * The makefile's default and `build` targets must build a kernel object file named `kdlp.ko`

        * The makefile's `clean` target must remove the module build artifacts and should return success even when no artifacts exist to be deleted

        * The makefile will need to reference the kernel makefile to properly build the out of tree module

            * Follow the [guidelines for recursive makefiles](https://www.gnu.org/software/make/manual/html_node/Recursion.html)

            * Pass appropriate defaults for ARCH and CROSS_COMPILE so that they do not need to be passed when invoking make in the module directory

    * Find and examine the kernel code for implementing `/proc/cmdline` and `/proc/config.gz` which demonstrate creating `/proc` entries that provide access to read-only data

        * Use the newer, better `proc_create_single()` API from the `/proc/cmdline` code

        * However, reference the code for `/proc/config.gz` for information on how to remove a `/proc` entry so that you can make your module unloadable

    * Don't overcomplicate things

        * Hint: You can do this with just three functions: init, exit, and one that generates the data

0. Test your work and document your findings

    * Build the testing program and the module using the makefiles

        * Copy the compiled program and the .ko file into the rootfs directory

    * Rebuild the rootfs

        * make an empty directory named `proc` in `rootfs` to act as the mount target for the proc filesystem

        * Add the following line to `etc/inittab` to mount the proc filesystem during startup

        ::sysinit:/bin/mount -t proc proc /proc

    * Boot into the new kernel capturing output into the `testing_output.txt` file

        * Run the testing program once before loading the module to show that it fails

        * Load the module using the `insmod` command

        * Re-run the testing program to show that it passes all the tests

        * Remove the module with the `rmmod` command

        * Re-run the testing program for a third time to verify that it no longer passes

0. Extra Credit Opportunity

    * Reimplement the module in rust and verify that it still passes your tests

    * You will need to install the rust compiler and clang using dnf

        * Run `podman exec -itu0 kdlp bash` to spawn a root shell that has the appropriate permissions

        * Use `dnf -y install llvm lld clang rustc rust-src bindgen-cli rustfmt` to get the appropriate utilities

        * Any packages installed after the container build will be lost if you restart the container, though you can always re-install them

    * You will need to rebuild the kernel with `LLVM=1` instead of `CROSS_COMPILE=riscv64-linux-gnu-`

        * Make sure to use `ARCH=riscv make clean` to get rid of everything first

        * Update the module.config file and add `CONFIG_RUST=y`

            * After rebuilding using `allnoconfig` make sure that CONFIG_RUST is actually set in `.config`

            * If it isn't, something is wrong with your toolchain

        * Rebuild the `Image` and `modules` targets

    * Create a new `rust` folder in your directory for this assignment adding a new `Makefile` and `kdlp.rs` file

        * The makefile will be similar except for the adjustments to the variables passed to the recursive make

        * You will need to create some of your own rust bindings for the C functions related to manipulating entries in the procfs

[Frequently Asked Questions](/faq.md)
