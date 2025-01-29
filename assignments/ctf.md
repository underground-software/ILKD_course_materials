## Capture the Flag 🏁

Solve a puzzle and uncover the secret message by demonstrating your knowledge of Linux.

#### Outcomes:

* Learn about the system calls commonly implemented by character devices and how they interact with the file position stored in the kernel

* Get familiar with how userspace system calls invoke kernel code within a driver

* Have fun solving a custom puzzle

#### What to submit:

* Patch 1 adds `username/ctf/ctf.c` and `username/ctf/Makefile`, each as unmodified copies of `ctf/ctf.c` and `ctf/Makefile` respectively

    * The latest commit to the ILKD_Submissions repo contains a directory named `username/ctf`

    * Copy `ctf/ctf.c` and `ctf/Makefile` into `username/ctf`

    * Add the files to git and make your first commit

* Patch 2 adds `username/ctf/solution/solution.c`, your solution program,
and a makefile `username/ctf/solution/Makefile` to build `solution.c` into an executable named `solution` via the default target

    * Write a C program that performs the right sequence of operations on `/dev/ctf` so that the messages from the driver in `dmesg` match the provided output in `dmesg_log`

    * Each successful operation will return one byte of the secret message

    * Your C program should collect all returned bytes and print them at the end

    * When you solve the puzzle, printing these bytes will reveal your secret message

    * If the arguments passed to the operation are incorrect, the returned byte will be nonsense

    * If the operation would leave the device in an invalid state (i.e. `f_pos` outside of the range `0` through `256`), the operation will return an error

* Don't forget a cover letter

    * Make sure to include the secret message you obtained in your cover letter

* All email code patches are expected to pass `checkpatch.pl` as described in the [policies and procedures](/procedures.md)

* Submit your patches to ctf@COURSE_DOMAIN

### Procedure:

0. Copy the driver code and makefile into your folder

    * Examine the driver code to understand how information flows from one function to another

        * How are the values printed to the kernel ring buffer generated?

        * How can you reverse-engineer that process?

0. Write your C program to perform the operations and gather the secret message

    * Initially, the program should just open `/dev/ctf`, verify that it gets a valid file descriptor, and then close the file

    * Between those two operations, insert code that calls `read(2)`, `write(2)`, `lseek(2)`, and `ioctl(2)` with correct arguments to decrypt the message

    * Compare the `dmesg` output from running your program to `dmesg_log` to see if you are on the right track

    * This driver breaks a number of conventions to simplify this exercise

        * The buffer argument for calls to `read(2)` and `write(2)` can be `NULL` since the driver does not actually read or write data to or from userspace

        * The non-error return values from the system calls will not obey the rules specified in their manpages

[Policies & Procedures](/faq/procedures.md)
