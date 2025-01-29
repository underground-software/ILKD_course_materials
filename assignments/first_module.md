## Your first kernel module 🍿

You will specify, implement, and test
a simple kernel module that creates a `kdlp` entry
in the Linux kernel's
[`/proc`](https://docs.kernel.org/filesystems/proc.html)
filesystem with a basic read only interface
that returns a unchanging message which includes the student's name.

#### Outcomes:

* Get familiar with the internals of Linux's
[`/proc`](https://docs.kernel.org/filesystems/proc.html) filesystem

* Learn how to specify the behavior of a kernel module
with a guided example

* Get familiar with software testing,
how to write good tests, and how to develop code using tests as a guide

* Write your first kernel module!

#### What to submit:

* Patch 1 adds `username/first_module/specification.txt`, a filled out copy of `first_module.txt`

* Patch 2 adds `username/first_module/tests/test.c`, your original testing program, and `username/first_module/tests/Makefile`, which builds your testing program as `test` via the default and `test` targets

* Patch 3 adds `username/first_module/kdlp.c` and a new makefile to build `username/first_module/kdlp.ko` via the default and `module` targets

* Don't forget a cover letter

* Submit your patches to first_module@COURSE_DOMAIN

#### Procedure:

0. Create your `username/first_module` directory

    0. Copy `first_module.txt` from the repository root as `username/first_module/specification.txt`

0. Edit your copy of `specification.txt` and fill in the blanks
to fully specify the expected behavior of your module.

0. Write a testing program and makefile
that exercises all the functionality your module will have
according to the specification
by performing operations on the file
that it will create in `/proc`

    * Create a `tests/` directory in `username/first_module`

    * Your test program must be named `test.c`
    and placed in `username/first_module/tests/`

    * Create a makefile in your `username/first_module/tests/`
    directory that builds an executable
    named `test` as the default target and `test` target

    * The makefile's `clean` target must remove the test
    build artifacts and should return success even when no
    artifacts exist to be deleted

    * The standard output from your testing program must
    comply with the Test Anything Protocol version 14.
    See <https://testanything.org/> for more info.

    * Be sure to write tests for all the behavior
    (existence of the file, file permissions,
    behavior of reading, writing, seeking, etc.)

    * Make sure to also test error conditions
    to make sure strange user input to your module
    won't crash the system, e.g. a null or invalid pointer

    * You should have at least 10 distinct tests
    that the module can pass or fail

        * When run, the program should provide information
        about which tests succeeded or failed

        * If a test failed, include in the program output
        how the behavior differed from what was expected

    * At this point, you have nothing to test

        * In order to validate your testing program,
        temporarily change the hard-coded path of the file it opens

        * Instead of opening a file in `/proc`,
        you can have it open a regular file
        whose contents/owner/permissions
        you can change manually
        to see which tests pass or fail

        * Don't forget to change this back before committing your code
        (or write your program / makefile
        in such a way that
        what file is used can be overridden
        from the default in `/proc` for testing)

0. Implement the module

    * The module source file must be named `kdlp.c`

    * The `/proc` file must be accessible at `/proc/kdlp`

    * Create a makefile in your `username/first_module/` directory to build/clean and load/unload your module

    * The makefile's default and `module` targets must build a kernel object file named `kdlp.ko`

    * The makefile's `load` and `unload` targets must load and unload your module respectively

    * The makefile's `clean` target must remove the module build artifacts and should return success even when no artifacts exist to be deleted

    * The kernel code in
[`fs/proc/cmdline.c`](https://elixir.bootlin.com/linux/v6.13/source/fs/proc/cmdline.c)
    and
[`kernel/configs.c`](https://elixir.bootlin.com/linux/v6.13/source/kernel/configs.c)
    contain examples that create simple `/proc` entries
    that providing access read-only data

    * Use the newer, better `proc_create_single()` API
    from the `/proc/cmdline` code

    * However, reference the code for `/proc/config.gz`
    for information on how to remove a `/proc` entry

    * Hint: You can do this with three functions:
    init, exit, and one that generates the data

    * Don't overcomplicate things

[Policies & Procedures](/faq/procedures.md)
