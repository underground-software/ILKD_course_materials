## F1 - Contributing to Open Source Software 🤝

You will receive a specification and tests created per [F0](specification.md).

You must implement the character device from spec as a Linux kernel module.

Your colleague's tests will pass when run against your implementation.

### Outcomes:

* Put your knowledge of character devices into practice

* Demonstrate your reading comprehension of technical topics

* Simulate real software engineering teamwork and communication

* Discern between your own errors and those of others while working with imperfect information

### What to submit:

* Patch 1 adds your assigned spec, tests, and makefile to the `username/character_device/tests` folder unmodified

    * You will be informed where to find your assigned spec, tests, and makefile

* Patch 2 fixes any bugs found in the assigned tests and specification

* Patch 3 adds your module as `username/character_device/driver.c` and a makefile `username/character_device/Makefile` that builds the module kernel object as the default target

    * The `clean` target of this makefile must remove all module build artifacts

* Patch 4 adds additional tests to the provided program

* Don't forget a cover letter

* Submit your patches to `character_device@fall2025-uml.kdlp.underground.software`

    * This assignment will not follow the normal three step submission process

        * You will only make one submission, by midnight the Monday before the first presentations take place

    * Your final presentation must demonstrate the exact same version of the code as in your F1 submission

### Procedure:

0. Locate and examine your assigned spec, tests, and makefile

    * The default target of the provided makefile should build the tests as a binary file `tests`

    * However, initially there is nothing to test

0. Carefully review the spec and examine each of the test cases

    * It will be helpful to begin by associating each test case with the corresponding part of the spec

    * If you find any issues, inconsistencies, or ambiguities in the spec or tests, then:

        * For simple problems, fix the issue yourself as you see fit

            * You may take some creative liberty but do your best to maintain the intent of the original author

        * For more puzzling concerns, you can post in the `#questions` channel on Matrix

    *  Think about whether the given test file tests all cases

        * This will give you a head start later when you are writing additional tests

    *  If you need to modify the spec, you may need to adjust some tests to reflect these updates

0. Using the now thoroughly reviewed spec, implement the character device it describes

    *  Use the kernel's [miscdevice API](https://www.kernel.org/doc/html/v6.16/driver-api/misc_devices.html) to register and de-register your character device

        * This is standard practice for simple kernel drivers implemented as character devices

    * Implement one syscall at a time and focus only on the tests pertaining to that syscall

    * To properly implement this concurrent behavior, you must enforce mutual exclusion around any access to shared data

        * Assume an arbitrary number of processes will be accessing your character device at the same time

        * Assume that any of the threads in these processes may be interrupted after any line of code for an arbitrary amount of time

        * Code designed to run in these conditions must be [reentrant](https://en.wikipedia.org/wiki/Reentrancy_(computing))

    * As always, no memory leaks, no use-after-free, no buffer overflows

0. Create 5 new tests

    * Try to add new tests for untested edge cases, areas of the spec that you may have changed, or more complicated integrations of multiple syscalls in a row

    * Your module added by the previous patch must pass these tests without further modification

[Frequently Asked Questions](/faq.md)
