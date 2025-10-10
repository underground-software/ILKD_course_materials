## Build a Shell 🐚

The objective of this assignment is to build a UNIX style shell.

For the purposes of grading, this assignment will be part of the "Programming Assignments" category.

### Outcomes:

* Familiarize yourself with the role of the C library vs the kernel in writing Linux applications

* Get hands on experience with signals, system calls, processes, and file descriptors

### What to submit:

* Patch 1 includes all of the files to build your finished shell in `submissions/$USER/shell/`

* Patch 2 demonstrates the working shell by including within `submissions/$USER/shell/output.txt` the complete output of launching your shell in `qemu` and running the test commands

* Don't forget a cover letter

* Submit your patches to `shell@fall2025-uml.kdlp.underground.software`

### Procedure:

0. Flesh out the rootfs with utilities from `busybox` you can launch with your shell

    * Clone our fork of busybox from the repositories on the class website and cd into the folder

            cd
            cp submissions/shell/busybox.config .
            git clone https://fall2025-uml.kdlp.underground.software/cgit/busybox
            cd busybox

    * Generate a minimal config based on the provided seed

            CROSS_COMPILE=riscv64-linux-gnu- make KCONFIG_ALLCONFIG=../busybox.config allnoconfig

    * Compile busybox. The existing config assumes you have the `sysroot` with libc from the previous assignment in `~`

            CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc)

    * Clean out the existing rootfs and make new folders

            cd
            rm -r rootfs
            mkdir -p rootfs/{,usr/}{s,}bin rootfs/etc

    * Copy in the compiled busybox binary

            cp busybox/busybox rootfs/usr/bin/busybox

    * Copy the provided inittab file that configures how the system boots

            cp submissions/shell/inittab rootfs/etc/inittab

    * Set BusyBox as the init program by creating a symlink

            ln -s ../usr/bin/busybox rootfs/sbin/init

0. Create your shell program that will act as `/bin/sh`

    * Do your work in the appropriate folder `~/submissions/$USER/shell` as with all other assignments

    * Create a makefile to compile your code with the appropriate options

        * You will need the same ones you used to compile your C code for the `new_syscall` assignment

        * You probably want to use multiple files to organize your code, so write a more advanced makefile that can build them all into one binary

        * The default target when `make` is run without any arguments should build a binary named `shell` that is your shell

        * Be sure to include appropriate standard `.PHONY` make targets like `all` and `clean` that do the right thing

    * Implement the features outlined below. They are layed out in order increasing in sophisitication, you should complete and thoroughly test each bullet point before moving on to the next, but it might be helpful to read them all before starting to get the big picture

    * You can test the shell by running it inside the container without needing to set up the VM by relying on qemu userspace emulation with the `qemu-riscv64-static` command

        * You can pass `-g 1234` to make the emulator wait for a debugger connection and attach using the `gdb` command with `target remote localhost:1234` in a second shell inside the container

    * At the end you should have a shell that mostly matches the basic functionality of the one in your container and lets you use the vm interactively


    * Features:

        0. The shell prints a prompt, which consists of the absolute path the shell is currently running in (see `man 3 getcwd`) followed by a `$` and a space (e.g. `/your/current/directory$`). The shell then prints a new line and exits without any user interaction.

        0. The shell reads lines of user input, but doesn't do anything with them. It just prints a new prompt before each line. This loops until the shell gets EOF from user input (ctrl+d), at which point it exits with code 0.

        0. If the user types anything, the shell prints "Unrecognized command" (but does not exit the loop). However, if the user just hits enter without typing anything, no error message is printed.

        0. The shell splits the line of input into pieces delimited by whitespace characters (see man 3 isspace). Instead of just printing "Unrecognized command," the shell shall include the name of the program in the error message (e.g. if the user types cat shell.c, the shell prints "Unrecognized command: cat"). If the user enters only whitespace, no error message is printed.

        0. The shell supports the following two builtin commands:

            * `exit` takes no arguments and closes the shell (see `man 2 exit`) with code 0

                * If arguments are provided, it prints an error message and does not exit the shell

            * `cd` takes exactly one argument and changes the working directory of the shell process to the provided path (see `man 2 chdir`)

                * If an incorrect number of arguments are provided, nothing happens and usage info is printed

                * If `chdir` fails, an error message is printed

            * `exec` takes *at least one* argument and replaces the shell with an instance of the executable file whose path is provided as the first argument (see man 2 execve)

                * It provides all its arguments as the `argv` for the called command, i.e. `argv[0]` is the first argument to `exec`, `argv[1]` is the second argument (if it exists), etc

                * If execve fails, an error message with a description of the errno is printed, and the shell continues running

        0. The shell supports running executable files as commands within child processes

            * If the first piece of the input looks like a relative or absolute path (contains a `/`) a child process is created:

                * The command specified by the first argument is executed within the child using all of the command's arguments (see `man 2 execve`)

                * Because the VM is running without a memory management unit, the traditional `fork` library function is not provided by libc as it is not possible to achieve its normal sematics without virtual memory

                * You have to use the raw kernel `clone` syscall

                * See `man 2 clone` paying special attention to the section "C library/kernel differences"

                * Even though a new process is being created, until `execve` or `exit` is called both processes are sharing an address space like two threads

                * Unlike two proper threads however, the two processes are also sharing a stack

                    * You almost certainly want to use the `CLONE_VFORK` flag to freeze the parent until the child exits or calls execve

                    * Be careful to never let the child process return past the function where clone was called or it will likely corrupt the stack that the parent will need

            * If executing the command fails, the child process prints an error message

                * Do not modify any global variables between clone and execve as they are shared between both processes

                    * This means you also cannot use many functions in the C library which implicitly modify global data (e.g. stdio which buffers output)

                * Don't forget to exit the child process

            * The shell waits for the child to finish running before printing the next prompt (see `man 2 wait4`).

        0. In the case that the user types something that isn't a path or a builtin, the shell checks whether a file with that name exists (see `man 2 access`) in any of the directories listed in the `PATH` environment variable in order

            * If a file with that name is found, the search can stop and that file is executed with arguments in a child process

            * If no file is found in any of the directories, an unrecognized command error is printed

                * For example `Unrecognized command: kdlp`

        0. Redirect the input/output if any redirections are provided in a given command.

            * `>` replaces stdout

            * `<` replaces stdin

            * If there are multiple of the same redirection, the right-most one takes precedence

            * A redirection causes the shell to open the corresponding file and replace one (or both) of the child process' IO streams with the file descriptor of the open file (see `man 2 dup3`)

            * For example

                * `echo hi > hi.txt` prints nothing to standard output, but creates a file `hi.txt` and writes `hi` to it

                * `cat < hi.txt` redirects the contents of hi.txt into standard in, and `cat` prints these contents to standard out

                * Redirections can be placed before, at the end of, or amidst a command's arguments, and they don't interfere with a command and its arguments

                    * `> count.txt wc < file.txt -l` is a perfectly valid way of running `wc -l` to count the lines in `file.txt` and output the result to `count.txt`

                * `echo hi > not_this_one.txt > or_this_one.txt > hi.txt` will create all three `.txt` files, but the output will only be redirected to hi.txt, as the other two output redirections are ignored

        0. The shell supports the `|` (pipe) operator to chain multiple commands and their inputs and outputs

            * Each command separated by a `|` is spawned as its own child process

            * The shell creates a unidirectional pipe (see `man 2 pipe2`) for each `|`, redirects the stdout from the left command to the writing end of the pipe, and redirects the stdin of the right command to the reading end of the pipe

            * Any file redirections specified by the user take precedence over the implied redirections from the `|`

    * Once you have a shell you are happy with, you can make your first commit

0. Try running your shell it in the VM

    * Compile your shell and copy the binary into the rootfs

            cd ~/submissions/$USER/shell
            make
            cp shell ~/rootfs/bin/sh

    * Rebuild the rootfs and boot the kernel.

        * You should see output from busybox's init program first and then it will spawn your shell

        * You should be able to explore the files in the vm using `ls` and `cd`

        * Run `busybox` by itself to get a list of all of the supported commands

            * Some of them might not work yet but most should

        * You can properly poweroff and reboot the vm using `poweroff` and `reboot`

    * Re run `qemu` and capture the output for your seconnd commit.

        *  Run the following commands

                cd bin
                ./uname -m | od -t d1 -An -vw0 | /usr/bin/sort -n | tail -n 1 > a.txt
                dmesg | tail -n +2 | head > b.txt
                cat b.txt - < a.txt | ../../../usr/bin/busybox sha256sum
                exec /sbin/poweroff

    * You may now make your second commit

[Frequently Asked Questions](/faq.md)
