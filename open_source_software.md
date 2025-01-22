## Contributing to Open Source Software 🤝

You will receive a specification and tests created per
[specification](specification.md)
by yourself or one of your colleagues.

You must implement the character device from spec as a Linux kernel module.

Your colleague's tests will pass when run against your implementation.

### Outcomes:

* Put your knowledge of character devices into practice

* Demonstrate your reading comprehension of technical topics

* Simulate real software engineering teamwork and communication

* Discern between your own errors and those of others while working with imperfect information

### What to submit:

* Patch 1 adds your assigned spec, tests, and makefile to the `username/open_source_software/tests` and fixes any bugs found in the assigned tests

    * You will be informed where to find your assigned spec, tests, and makefile

* Patch 2 adds your module as `username/open_source_software/module.c` and a makefile `username/open_source_software/Makefile` that builds the module kernel object as the default target

    * The `clean` target of this makefile must remove all module build artifacts

    * The `load` and `unload` targets of this makefile must load and unload your module, respectively

* Patch 3 adds additional tests to the provided program

* Don’t forget a cover letter

* All email code patches are expected to pass `checkpatch.pl` as described in the [policies and procedures](/procedures.md)

* Submit your patches to `open_source_software@COURSE_DOMAIN`

    * Your final presentation must use the exact same version of the code as your final submission

### Procedure:

0. Locate and examine your assigned spec, tests, and makefile

	*  The default target of the provided makefile should build the tests as a binary file `tests`

	*  However, initially there is nothing to test

0. Carefully review the spec and examine each of the test cases

	*  It will be helpful to begin by associating each test case with the corresponding part of the spec

	*  If you find any issues, inconsistencies, or ambiguities in the spec or tests, then:

        * For simple problems, fix the issue yourself as you see fit

        * You may take some creative liberty but do your best to maintain the intent of the original author

        * For more puzzling concerns, post in the #questions channel on KDLP Matrix

	*  Think about whether the given test file tests all cases

        * This will give you a head start later when you are writing additional tests

	*  If you modify the spec, you may need to adjust some tests to reflect these updates

0. Using the now thoroughly reviewed spec, implement the character device it describes

	*  Use the kernel's
[miscdevice API](https://www.kernel.org/doc/html/v6.13/driver-api/misc_devices.html)
to register and de-register your character device

        * This is standard practice for simple kernel drivers implemented as character devices

	* Implement one syscall at a time and focus only on the tests pertaining to that syscall

	* Assume an arbitrary number of processes will be accessing your character device at the same time

	* Assume that any of the threads in these processes may be interrupted after any line of code for an arbitrary amount of time

	* Code designed to run in these conditions must be
[reentrant](https://en.wikipedia.org/wiki/Reentrancy_(computing))

	* To properly implement this concurrent behavior, you must enforce mutual exclusion around any access to shared data

	* As always, no memory leaks, no use-after-free, no buffer overflows

0. Create 5 new tests

	* Try to add new tests for untested edge cases, areas of the spec that you may have changed, or more complicated integrations of multiple syscalls in a row

	* Your module added by the previous patch must pass these tests without further modification

[Policies & Procedures](/procedures.md)
