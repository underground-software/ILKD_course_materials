## F0 - Write a Novel Specification 📐

You will conceive of an idea for a novel character device with the objective of designing something that is creative and non-trivial and will provide for an entertaining demonstration as part of someone's final presentation.

With the knowledge that it is one of your colleagues who will be responsible for actually implementing your idea, you will write a specification of the behavior of each system call that is precise and detailed enough for them to implement it correctly.

To further guide your colleague in implementing a device that complies with your creative vision, you must write a userspace program that executes a suite of tests that a correct implementation should pass.

For F1, you will randomly be assigned one of the specifications submitted for F0.

### Outcomes:

* Validate your conceptual understanding of character devices

* Demonstrate your creative capacity and vision

* Simulate teamwork and communication aspects of working on a real-world coding team

* Practice clarity in technical writing by clearly specifying the intended behavior of code to colleagues

* Further develop your ability to rigorously test code

### What to submit:

* Patch 1 adds `username/specification/spec.txt`, containing your specification

* Patch 2 adds your test program `username/specification/test.c` and a makefile `username/specification/Makefile` that builds an executable binary named `test` as the default target

* Don't forget a cover letter

* Submit your patches to specification@fall2025-uml.kdlp.underground.software

### Procedure:

0. Brainstorm some ideas with an eye for how the behavior can be implemented by a real character device

    * Get started early so that you don't have to pick the first idea you can come up with right before the due date

        * Some entertaining ideas may be more feasible on paper than in practice

    * As a source of inspiration, here are some categories of ideas that have proved viable in the past

        * A simple game, like tic-tac-toe

            * This specific topic (tic-tac-toe) is off limits since we will be using it as an example in class

        * A tool such as a unit converter or cipher encoder/decoder

        * A simulator for some process e.g. ordering food at a drive-thru window, or a Tamagotchi-pet, or interpreting a simple programming language

0. The most important system calls to figure out will be `read(2)`, `write(2)` and `ioctl(2)`

    * `read(2)` and `write(2)` are best at handling strings of text

        * You should be aim to design a device that can be "played with" interactively using tools like `cat` to read from it and `echo` with shell redirection to write to it

    * `ioctl(2)` can be used as a bit of a catch-all mechanism for operations on the device because it can be used to send and/or receive data arbitrary data via a pointer to a structure

        * That being said, there is not an easy way to play with the device using ioctl in the same way that you can with read/write using `cat` and `echo`

        * The interface of commands implemented by your `ioctl(2)` must be defined using the appropriate `_IO*` macros and provided to userspace in a header file

0. Once you have designed the core functionality of the device, consider how those functions will (or will not) interact with the file position value that the kernel stores for each open file and how `lseek` will fit into that picture

    * What if any types of repositioning should be supported?

    * How will the other functions handle a file position that might be out of bounds?

0. Finally, you have to think about the resource lifecycle within the module

    * Is there some persistent state the whole time the module is loaded?

        * How should device state be initialized at load time in your `init()`

    * What about state specific to a given file descriptor?

        * How should the file state be initialized when the device file is opened in your `open()` function?

0. Write a plain-text specification for the `open(2)`, `read(2)`, `write(2)`, `close(2)`, `ioctl(2)`, and `lseek(2)` system calls

    * You must describe how a user can correctly invoke each syscall and what behavior or information it accepts and/or returns

    * You must describe the behavior and `errno` values returned for all possible failure modes that come to mind

    * You must describe with as much precision as possible the state of the system following a successful invocation of any of the features

0. Write a program that implements a minimum of 25 tests

    * Your test program should run all tests with a single invocation

    * Standard output from the binary must comply with the [test anything protocol (TAP)](https://testanything.org/)

        * In case of unexpected behavior, include in the output of the testing program how the behavior differed from what was expected

    * As a part of the minimum of 25, you must write at least 5 tests for each of the system calls `read(2)`, `write(2)`, and `ioctl(2)`

        * Be sure to write tests that cover all of the behavior you specified, including but not limited to:

            * Simple tests that try to isolate a specific behavior and verify it is correct

            * More complex integration tests that verify that different system calls can work together

            * All failure modes that happen within the logic of the character device or as a result of user behavior

                * That is to say, not ones resulting from extreme system conditions beyond the module such as lack of resources

        * Get creative when considering the incorrect usage of your API

            * Come up with some exotic input data and insane parameters

            * Don't forget to test for simple memory errors like usage of a null or invalid pointer

        * Your test program should gracefully handle errors, e.g. it should not crash if a test fails or if the character device's special file cannot be found

            * You can report tests as skipped within TAP

            * You should only use the `"Bail out!"` directive from TAP to deal with unrecoverable errors in the testing code itself (e.g. malloc returned null) and not for errors in the code under test

0. You will not have an implementation to run your test program against, however you can still verify that it is performing the right operations by pointing it at a dummy file like `/dev/null` or `/dev/zero` and verifying that it is making the right system calls with `strace`

    * Be careful while writing your tests and document them well so that the person implementing the module can understand your thinking

    * We will not take off points for honest mistakes, but we do reserve the right to take points off if the tests are completely incoherent and prove impossible to work with

[Frequently Asked Questions](/faq.md)
