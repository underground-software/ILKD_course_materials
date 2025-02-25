## Build a Shell 🐚

The objective of this assignment is to build a UNIX style shell.

### Outcomes:

* Familiarize yourself with the role of the C library in writing Linux applications

* Get hands on experience with signals, system calls, processes, and file descriptors

### What to submit:

* Patch 1 includes all of the files to build your finished shell in `submissions/$USER/shell/`

* Patch 2 demonstrates the working shell by including the complete output of launching your shell in `qemu` and running the following sequence of commands in the file `submissions/$USER/shell/output.txt`

        cd bin
        uname -m | od -t d1 -An -vw0 | /bin/sort -n | tail -n 1> a.txt | dmesg | tail -n +2 | head > b.txt | cat b.txt - < a.txt | ../../../bin/busybox sha256sum
        exit

* Don't forget a cover letter

* Submit your patches to `shell@spring2025-utsa.kdlp.underground.software`

### Procedure:

0. Download the provided starter files into your container

    * `mkdir submissions/$USER/shell/ && cd submissions/$USER/shell`

    * `wget spring2025-utsa.kdlp.underground.software/assignment_materials/shell/parse.h`

    * `wget spring2025-utsa.kdlp.underground.software/assignment_materials/shell/parse.c`

    * `wget spring2025-utsa.kdlp.underground.software/assignment_materials/shell/busybin.tar`

    * `mkdir rootfs && tar -xf busybin.tar -C ./rootfs && rm busybin.tar`

        * `busybin.tar` contains a bunch of standard utilities for you to test your shell with in your Qemu virtual machine

0. Create a makefile (`submissions/$USER/shell/Makefile`)

    * Use `riscv-linux-gnu-gcc` to compile

    * Compile with the following flags:

            -std=c23 -Wall -Wextra -ffreestanding -nostdlib -march=rv64im -mabi=lp64

0. Set your shell as the init program and pass your shell a PATH environment variable

    * When running `qemu`, include the option `-append 'PATH=/sbin:/bin:/usr/sbin:/usr/bin init=/shell'` to provide kernel command line arguments

    * `PATH` is an environment variable that tells your shell where to look for programs

    * `envp` contains a list of environment variable strings that starts after `argv` on the stack, and is likewise terminated by a null pointer

    * `init=/shell` tells the kernel to use `/shell` as the init program rather than the default `/init`

0. Implement the features outlined below. This assignment will guide you through the process of building a shell by iteratively improving
your code and adding more features, starting with a very simple program

    * At the end you should have a shell that mostly works

    * Building a shell requires the use of several system calls. Since you don't have a C library, you will need to make system calls
    directly to the linux kernel yourself. We suggest writing a `syscall` function in assembly or in inline assembly, and writing wrapper
    functions around calls to your `syscall` function for each call you want to make

        * This improves readability and makes it easier to avoid silly mistakes like flipping the order of arguments by providing some type safety

    * Refer to the kernel `scripts/syscall.tbl` table for system call numbers

    * Read `man` entries or find the appropriate `SYSCALL_DEFINE`s in the kernel source or at https://elixir.bootlin.com/linux/v6.13/source
    for reference

    * Oftentimes the C library function for making a system call is not 1:1 with the linux kernel interface

        * For example the C library function `fork` makes a call to the `clone` system call

        * Try writing and `strace`-ing test programs in your host contianer to see what the C library is doing behind the scenes

### Features:

0. The shell prints a prompt (see `man 2 write`), which consists of the absolute path the shell is currently
running in (see `man 2 getcwd`) followed by a $ and a space (e.g. `/your/current/directory$ `)

0. The shell reads lines of user input and calls the provided starter code to parse the input

    * Since you don't have a memory allocator, read input (see `man 2 read`) into a statically sized buffer

    * If parsing a line of input fails, print an error message and prompt for input again

0. If the user hits enter without typing anything, nothing happens and the prompt is printed again

    * If the shell reads `EOF` (end of file), the shell exits

0. The shell supports the following two builtin commands:

    * `exit` takes no arguments and closes the shell (see `man 2 exit`) with code 0

        * If arguments are provided, it prints an error message and does not exit the shell

    * `cd` takes exactly one argument and changes the working directory of the shell process to the provided path (see `man 2 chdir`)

        * If an incorrect number of arguments are provided, nothing happens and usage info is printed

        * If `chdir` fails, an error message is printed

0. The shell supports running executable files as commands within child processes

    * If the first piece of the input looks like a relative or absolute path (contains a `/`):

        * A child process is created (see `man 2 clone` and pay special attention to the "C library/kernel differences" section)

        * The command specified by the first argument is executed within the child using all of the command's arguments (see `man 2 execve`)

    * If executing the command fails, the child process prints an error message

        * Don't forget to exit the child process

    * The shell waits for the child to finish running before printing the next prompt
    (see `man 2 wait4`).

0. In the case that the user types something that isn't a path or a builtin,
the shell checks whether a file with that name exists (see `man 2 access`) in any of the directories listed
in the `PATH` environment variable in order

    * If a file with that name is found, the search can stop and that file is executed with
    arguments in a child process

    * If no file is found in any of the directories, an unrecognized command error is printed

        * For example `Unrecognized command: kdlp`

0. Redirect the input/output if any redirections are provided in a given command.

    * `>` replaces stdout

    * `<` replaces stdin

    * If there are multiple of the same redirection, the right-most one takes precedence

    * A redirection causes the shell to open the corresponding file and replace one (or both)
    of the child process' IO streams with the file descriptor of the open file
    (see `man 2 dup3`)

    * For example

        * `echo hi > hi.txt` prints nothing to standard output, but creates a file
        `hi.txt` and writes `hi` to it

        * `cat < hi.txt` redirects the contents of hi.txt into standard in, and `cat` prints these
        contents to standard out

        * Redirections can be placed before, at the end of, or amidst a command's arguments, and they don't interfere with a
        command and its arguments

            * `> count.txt wc < file.txt -l` is a perfectly valid way of running `wc -l` to count the lines in
            `file.txt` and output the result to `count.txt`

        * `echo hi > not_this_one.txt > or_this_one.txt > hi.txt` will only create all three `.txt` files, but the output will only be redirected to hi.txt,
        as the other two output redirections are ignored

0. The shell supports the `|` (pipe) operator to chain multiple commands and their
inputs and outputs

    * Each command separated by a `|` is spawned as its own child process

    * The shell creates a unidirectional pipe (see `man 2 pipe2`) for each `|`, redirects
    the stdout from the left command to the writing end of the pipe, and redirects the stdin
    of the right command to the reading end of the pipe

    * Any file redirections specified by the user take precedence over the implied
    redirections from the `|`

[Frequently Asked Questions](/faq.md)
