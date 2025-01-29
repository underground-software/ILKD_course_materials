## Scavenger Hunt 🔎

We will assign you a system call and give you a specially designed program.
Your task is to trace invocations of that system call by the program within
userspace and kernelspace and document your observations.

#### Outcomes:

* Strengthen understanding of the interaction between the kernel and userspace

* Learn how to monitor a program's execution of system calls

* Build awareness of, and basic familiarity with, the linux BPF subsystem via `bpftrace`

* Gain experience with tracing based debugging

#### What to submit:

* Patch 1 adds `username/scavenger_hunt/questions.txt`, an unmodified copy of `scavenger_hunt/questions.txt`

* Patch 2 adds `username/scavenger_hunt/strace_user` and `username/scavenger_hunt/strace_root`, your two logs

    * Each log contains the full output from running `strace` on the special program

    * `strace_user` contains the output from running the program as an unpriveleged user

    * `strace_root` contains the output from running as root using `sudo`

* Patch 3 adds `username/scavenger_hunt/trace.bp`, your original, executable `bpftrace` script, and a file `username/scavenger_hunt/trace.out` that shows the expected output of a successful trace of the special program making your system call

* Patch 4 modifies `username/scavenger_hunt/questions.txt` to answer each question

* Patch 5 adds `username/scavenger_hunt/log.txt` with the output from all the times you ran the program

* Don't forget a cover letter

* Submit your patches to scavenger_hunt@COURSE_DOMAIN

#### Procedure:

0. Create an `scavenger_hunt` directory in your `username` directory

0. Copy the provided `scavenger_hunt/questions.txt` file into your `username/scavenger_hunt`

0. Play with the provided program, either `program_amd64` or `program_arm64` depending on your architecture

    0. Regardless of the location of the binary, the program will create a `log.txt` file in the directory where the program is invoked

    0. You will need this output for patch 5

0. Disassemble the appropriate binary using a tool like `objdump -d <binary>`

    0. Identify the lines of code that prepare the arguments for your system call, specify the system call number, and invoke the kernel

    0. Use this to answer question 0 for your architecture

0. Run `strace` on the provided program

	0. Locate the lines corresponding to call(s) to your system call in the output

    0. Use this to answer question 1

0. Read the manual page for your system call (type `man 2 [system call name]` e.g. `man 2 execve`)

    0. Use this to answer questions 2, 3, and 4

0. Analyze the disassembled binary to answer question 5

0. Find the system call definition within the kernel source

	0. Suggested: make use of `git grep` or [elixir](https://elixir.bootlin.com/) or [cscope](http://cscope.sourceforge.net/)

    0. Use this to answer question 6

0. Pull the full history of the Linux kernel repository

    0. Make sure you have plenty of free space on your disk

    0. Use `git pull --unshallow` (this will take a little while to download)

0. Run `git blame` on the file containing the definition of the system call

	0. Find the most recently modified line or lines in the system call handler

    0. Use this to answer questions 7, 8, 9, and 10

0. Identify the probe you will use for tracing by searching for available probes that match your system call with `sudo bpftrace -l`

0. Write the BPF script that inserts a probe triggered by the program running your system call

    0. The program prints the system call arguments, the kernelspace stack, and the userspace stack each time the system call is executed by the program

	0. Ensure that the probe is only triggered on instances of the system call caused by running the provided program

0. Capture the necessary output, and create commits out of your changes as described above

0. Generate patches out of your commits

#### Tools of interest

* `strace`

* `man`

* `cscope`

* `git-blame`

* `git-grep [ -n ]`

* `bpftrace` and other BPF tooling

[Policies & Procedures](/faq/procedures.md)
