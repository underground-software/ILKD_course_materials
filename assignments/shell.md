## Build a Shell 🐚

The objective of this assignment is to build a UNIX style shell.

### Outcomes:

* Exercise your practical knowledge of Linux systems programming

### What to submit:

* Patch 1 includes all of the files to build your finished shell in `submissions/$USER/shell/`

* Patch 2 FIXME proves that it works somehow in a `submissions/$USER/shell/proof.txt` file

* Don't forget a cover letter

* Submit your patches to `shell@spring2025-utsa.kdlp.underground.software`

### Procedure:

0. Download the provided starter files into your container

    * FIXME instructions to download

    * Put the provided `.c` and `.h` starter files in your `submissions/$USER/shell/` directory

    * Put the provided `bin` folder into your `~/rootfs` directory

        * This folder contains a bunch of standard utilities for you to test your shell with in your Qemu virtual machine

0. Create a makefile (`submissions/$USER/shell/Makefile`)

    * Use `riscv-linux-gnu-gcc` to compile

    * Compile with the following flags:

            -std=c2x -Wall -Wextra -ffreestanding -nostdlib -fpic -fpie -static -shared -fvisibility=hidden -Wno-implicit-fallthrough -march=rv64im -mabi=lp64
            FIXME double check these are the flags we want them to use

0. Set your shell as the init program and pass your shell a PATH environment variable

    * In the linux kernel config menu, navigate to the `Boot Options` menu

    * Select `Built-in kernel command line` and enter `init=/bin/shell PATH=/bin`

        * Make sure the field `Built-in command line usage` is set to `Use bootloader kernel arguments if available` FIXME or a different setting?

    * `PATH` is an environment variable that tells your shell where to look for programs

        * The provided starter code passes environment variables to the `main` function in the `char **envp` parameter

        * `envp` is a list of strings representing environment variables

            * `envp` is terminated by a null pointer

            * For example: `{"HOME=/", "TERM=linux", "PATH=/bin", NULL}`

        * `PATH` is usually a list of paths delimited by `:`s

            * Try `echo $PATH` in your host container to see an example

    * `init=/bin/shell` tells the kernel to use `/bin/shell` as the init program rather than the default `/init`

    * Make sure to remove any old `init` programs from your `rootfs` folder

0. Implement the features outlined below. This assignment will guide you through the process of building a shell by iteratively improving
your code and adding more features, starting with a very simple program

    * At the end you should have a shell that mostly works

    * To test your code quickly, you can compile your `shell` binary like normal, and instead of loading it into your rootfs
    and running it as the init program in your Qemu virtual machine, you can run `./shell` from your host container and Qemu will
    handle translating and running the binary

    * Building a shell requires the use of several system calls. Since you don't have a C library, you will need to make system calls
    directly to the linux kernel yourself. We suggest writing wrapper functions around calls to the provided `syscall` macro

    * Refer to the kernel `scripts/syscall.tbl` table for system call numbers

    * Read `man` entries or find the appropriate `SYSCALL_DEFINE`s in the kernel source or at https://elixir.bootlin.com/linux/v6.13.3/source
    for reference

        * FIXME: the student containers dont have man, push an update or direct them to google

    * Oftentimes the C library function for making a system call is not 1:1 with the linux kernel interface

        * For example on RISCV the C library function `fork` makes a call to the `clone` system call

        * Try writing and `strace`-ing test programs to see what the C library is doing behind the scenes

            * Make sure to cross-compile your test programs to RISCV

### Features:

0. The shell prints a prompt (see `man 2 write`), which consists of the absolute path the shell is currently
running in (see `man 3 getcwd`) followed by a $ and a space (e.g. `/your/current/directory$ `)

0. The shell reads lines of user input and calls the provided starter code to parse the input

    * Since you don't have a memory allocator, read input (see `man 3 read`) into a statically sized buffer

        * Your shell only needs to handle lines of input that are 512 chars or fewer in length

    * If parsing a line of input fails, print an error message and prompt for input again

0. If the user hits enter without typing anything, nothing happens and the prompt is printed again

    * If the shell reads `EOF` (end of file), the shell exits

0. The shell supports the following two builtin commands:

    * `exit` takes no arguments and closes the shell (see `man 3 exit`) with code 0

        * If arguments are provided, it prints an error message and does not exit the shell

    * `cd` takes exactly one argument and changes the working directory of the shell process to the provided path (see `man 2 chdir`)

        * If an incorrect number of arguments are provided, nothing happens and usage info is printed

        * If `chdir` fails, an error message is printed

0. The shell supports running executable files as commands within child processes

    * If the first piece of the input looks like a relative or absolute path (contains a `/`),
    a child process is created (see `man 2 fork` and `sys_clone`), and the command specified by the first
    argument is executed within the child using all of the command's arguments (see `man 2 execve`)

    * If executing the command fails, the child process prints an error message

        * Don't forget to exit the child process

    * The shell waits for the child to finish running before printing the next prompt
    (see `man 2 waitpid` and `sys_wait4`).

0. In the case that the user types something that isn't a path or a builtin,
the shell checks whether a file with that name exists in any of the directories listed
in the `PATH` environment variable in order (see `man 2 stat` and `sys_statfs`)

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
    (see `man 2 dup2` and `sys_dup3`)

    * For example

        * `echo hi > hi.txt` prints nothing to standard output, but creates a file
        `hi.txt` and writes `hi` to it

        * `cat < hi.txt` redirects the contents of hi.txt into standard in, and `cat` prints these
        contents to standard out

        * Redirections can be placed before, at the end of, or amidst a command's arguments, and they don't interfere with a
        command and its arguments

            * `> count.txt wc < file.txt -l` is a perfectly valid way of running `wc -l` to count the lines in
            `file.txt` and output the result to `count.txt`

        * `echo hi > not_this_one.txt > or_this_one.txt > hi.txt` will only create and redirect output to hi.txt, as the other two
        output redirections are ignored

0. The shell supports the `|` (pipe) operator to chain multiple commands and their
inputs and outputs

    * Each command separated by a `|` is spawned as its own child process

    * The shell creates a unidirectional pipe (see `man 2 pipe`) for each `|`, redirects
    the stdout from the left command to the writing end of the pipe, and redirects the stdin
    of the right command to the reading end of the pipe

    * Any file redirections specified by the user take precedence over the implied
    redirections from the `|`
