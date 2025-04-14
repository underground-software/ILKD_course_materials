## Final Project 🎯 🏹

This is your last opportunity to demonstrate your skills
and reflect on what you've learned in the course.

This project is worth 35% of your grade and includes a
component structured like an assignment but without peer
review and a live presentation to the class that will
include a coding challenge.

Your will submit the patchset described in the "What to submit"
by midnight the Tuesday before the first in class presentation,
that is to say, midnight April 22 2025.


### Outcomes:

* Put your knowledge of character devices into practice

* Demonstrate your reading comprehension of technical topics

* Simulate real software engineering teamwork and communication

* Practice your soft skills

* Give an engaging technical presentation

* Test your technical interview skills

* Reflect and connect your work to the bigger picture

### What to submit:

* Patch 1 adds your assigned spec, tests, and makefile to the `username/final/tests` and fixes any bugs found in the assigned tests

    * You will be informed where to find your assigned spec, tests, and makefile

* Patch 2 adds your module as `username/final/module.c` and a makefile `username/final/Makefile` that builds the module kernel object as the default target

    * The `clean` target of this makefile must remove all module build artifacts

    * The `load` and `unload` targets of this makefile must load and unload your module, respectively

* Patch 3 adds additional tests to the provided program

* Don’t forget a cover letter

* Submit your patches to `final@spring2025-utsa.kdlp.underground.software`

    * Your final presentation must use the exact same version of the code as your final submission

### Procedure:

0. Enable loadable modules, as well as the `proc`, `sys`, and `dev` file systems

    * In the Linux kernel `menuconfig`:

        * At the top level, select 'y' on "Enable loadable module support"

            * Navigate into the sub-menu for that item and select 'y' on "Module unloading"

        * Navigate to `File systems`->`Pseudo filesystems`

            * Enable `/proc file system support` and `sysfs file system support`

        * Return to the the main menu, then proceed to `Device Drivers`->`Generic Driver Options`

            * Enable `Maintain a devtmpfs filesystem...` and `Automount devtmpfs at /dev...`

        * Save and exit `menuconfig`

0. Compile your kernel using the new config

    0. Boot with this [new rootfs](/assignment_materials/final/rootfs.tar) to enable complete busybox functionality

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

	*  You must use the kernel's
[miscdevice API](https://www.kernel.org/doc/html/v6.13/driver-api/misc_devices.html)
to register and de-register your character device

        * This is standard practice for simple kernel drivers implemented as character devices

	* Implement one syscall at a time and focus only on the tests pertaining to that syscall

    * You should be able to use cat to read from your character device and echo with redirect to write to your character device

	* Assume an arbitrary number of processes will be accessing your character device at the same time

	* Assume that any of the threads in these processes may be interrupted after any line of code for an arbitrary amount of time

	* Code designed to run in these conditions must be
[reentrant](https://en.wikipedia.org/wiki/Reentrancy_(computing))

	* To properly implement this concurrent behavior, you must enforce mutual exclusion around any access to shared data

    * Assume any static data in your module is shared between an arbitrary number of concurrent threads!

	* As always, no memory leaks, no use-after-free, no buffer overflows

0. Create 5 new tests

	* Try to add new tests for untested edge cases, areas of the spec that you may have changed, or more complicated integrations of multiple syscalls in a row

	* Your module added by the previous patch must pass these tests without further modification

### Presentation requirements:

- 30-35 minutes long

- Start with course reflection of no more than 10 minutes

    - Be prepared to get cut off if you reach 10 minutes of reflection

- Demonstrate your code for 5 minutes

    - It is required and must include the components outlined below

- Live coding challenge for the rest of your time slot

    - We will ask you to make changes to your code live

    - This is similar to a coding interview where you are evaluated more for your thought process than whether you give a perfect answer

### What to present:

- Demonstrate your character device

    - You must present this section from your live terminal/editor

    - You must present the version of your module from your final submission unmodified

    - You must build, load, test, and unload the module

    - You must show cat and echo working on your module

    - You must discuss your design choices and how you approached the spec and tests you were given

    - You must explain and if possible demonstrate the thread and memory safety of your code

- Course reflection and feedback

    - You must present this section with slides

    - Choose your favorites from the prompts below

        - You will not be able to cover all of them in the time given

        - Pick a few that you think you can particularly speak to

    - Reflection prompts:

        - In your own words, what did you learn in this class?

            - Try and pick 2-3 important topics

            - How did your work on specific assignments build your understanding?

        - How does your work fit into the big picture of...?

            - Your academic career

            - Your software engineering skill development

            - Your understanding of open source software

        - What assignments did you particularly enjoy or not enjoy?

            - Explain why your felt that way

            - For those you enjoyed, how could you go further?

            - For those you did not enjoy, how could we improve?

        - Pick an assignment you struggled with:

            - How did you overcome your obstacles?

            - How will your approach differ if you did it again?

        - Pick an assignment you excelled at:

            - What made your work exceptional?

            - Do you have any advice for future students?

## Presentation Slots

#### L24: April 24

Slot 0: 3:30-4:05
Slot 1: 4:10-4:45

#### L25: April 29

Slot 2: 3:30-4:05
Slot 3: 4:10-4:45

#### L26: May 1

Slot 4: 3:30-4:05
Slot 5: 4:10-4:45

#### L27: May 6

Slot 6: 3:30-4:05
Slot 7: 4:10-4:45

#### L28: May 8

Slot 8: 3:30-4:05
Slot 9: 4:10-4:45

[Policies & Procedures](/faq/procedures.md)
