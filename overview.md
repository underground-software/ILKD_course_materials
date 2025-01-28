## Objectives:

* Develop Linux kernel internals knowledge

    - Major areas: MM, vfs, device drivers, IPC, networking

    * Create, develop, and test interesting and creative Linux kernel modules

    * Stretch their C programming skills to the limit

Covered by non programming assignments (peer review & presentations, plus cover letters)

* Develop the ability to autonomously participate in the open source software community

* Learn how to give and receive feedback on code patches

* Demonstrate the ability to explain your code and your knowledge of Linux in your own words

### What's the best stuff in the course??

scavenger_hunt is the best assignment

new_syscall is good
- need to edit places in the kernel
- edit C and assembly program
- maybe add more or make it early
- make second assigment?

### What should we nuke?


# IIT Spring outline:

minimal linux

kernelspace/userspace
system calls
interrupts
traps, etc. differences between arches
distinguish between hardware and software choices
Execution contexts: process vs atomic, also thread and other details


Advanced C features relative to the kernel
building up to understand arm64 entry code

Visibility, tracing, debugging
BPF stuff
ftrace
gdb with the kernel

the modular kernel
modules in isolation
loading and unloading
translating userspace program into module
character devices
timing and concurrency, kkey example

File descriptors via understanding syscalls in detail
open, close, read, write, ioctl, lseek

# Theoretical course outline
- Major areas: MM, vfs, device drivers, IPC, networking
- Other important areas: scheduling, filesystems

0. barebones linux

Both silent and deadly.

We have nothing. What's the smallest core we can start with?

extra-no allnoconfig
turn on expert mode, turn off more stuff

init program without syscalls
argument passed on kernel command line
data comes out via panic message

Now we have an operating system. But not much else.

1. GDB

Silent and deadly. What's going on?
What the hell is the kernel up to?

Enable GDB to allow us to see what the kernel is up to.

Now, we can step through the execution of kernel code

1. Logging with printk

The kernel runs. We can debug by freezing execution.
This is too slow. We want to let it rip and get output in real time.

Add the ability to log to the kernel ring buffer
What is the kernel ring buffer
dmesg

Now, we can do print debugging.

1. Memory management

Try to write programs that access memory. It's tough

Virtual to physical address identity mapping by default
What protections do we have?

turn on the MMU: Now we have virutal addresses. We can write some programs.

turn on ELF: now we can move beyond flat binaries

1. System calls

Currently, non-interractive
We want interactive computing. Throwback to pre-Unix

Hello world init program in assembly
call reboot
how are these implemented on different arches.

Trivial extension of systemcalls: a basic shell. Now we are interractive

1. Multiple users

Right now, anyone can walk in and burn the place down.
Login screen, username/password
We'd like to be able to run without privileges

Enable multiple users. Now we have some security

1. Shabangs

The kernel knows to run our programs due to magic bytes as seen in MMU above
There is a special case of these: shabang 

We want to be able to create and run programs from with the computer
Easy way is this

Enable shabangs, now we can run shell scripts

1. Modules

Right now, the kernel cannot be changed without recompiling

Enable modules: now we can add pieces of the kernel at runtime

1. /proc

What processes are running? We can't use ps since it reads from /proc

Enable /proc, now we can use ps

1. /sys

/proc is limited. There is a lot more we want to see and use.

Enable /sys, now we can use the sysrq triggers, the dtb, module info, and more

Lots of juice here

1. Storage

We have no persistence. Changes to our filesystem (rootfs) are runtime only.

Enable block devices. Add hard drive via QEMU. Now we have persistent storage

Maybe use virtio, or expose host folder as storage device from within VM

Look at /etc/fstab

1. Compiler & make

We can't build C programs from source. Let's bring in a compiler

Now, we can build the kernel within our VM

????

First goal: compile linux within our machine

Estimated to take 2-3 weeks of class

Adding historical stuff, demos, etc will extend substantially


Assignments in this section:
setup, new syscall, new module


Sections:

1. File descriptors and the VFS
    * maybe via syscall tracing

1. Device drivers
    * Interrupts
    * simulated harware with QEMU
    * Concurrency & reentracy

1. Debugging and tracing
    * BPF
    * ftrace
    * GDB
    * crash?
    
1. IPC and networking
    * signals
    * sockets from BSD
    * more

1. Containers
    * Namespaces
    * Cgroups
    * Podman & docker


### The work:

* Coming up with demos (2)
* Revising and creating assignments to re-inforce linux kernel internals knowledge (3)
* Building lecture around demos (2)
* Delivering the lectures (1)
* Creating text and video content for public consumption from lecture materials (5)
    * Lecture notes
    * Textbook chapters
* Managing grading operations and student communication (1)
* Developing singularity to support all the above (4)

### division

Charlie:
- demos

Joel:
- deliver lectures

Michael:
- updating assignments and potentially modifying/adding to them
- singularity autograder minor features as appropriate

Denis:
- Turning demos into lecture content

Sam
- Manage grader?
- turing lecture output into public content


#### Timeline:

demos have 2 week lead time

e.g:

Monday:
start demos for 2 weeks from this Tuesday/Thursday to be done by the following Monday
start converting demos to lecture for 1 week from this Tuesday/Thursday
start reviewing prepared lecture content for this week

Concrete:

21/23 Jan syllabus/policies/setup
28/30 Jan (setup due) barebones linux
04/06 Feb
11/13 Feb
18/20 Feb
25/27 Feb
04/06 Mar
11/13 Mar SPRING BREAK
18/19 Mar
25/27 Mar
01/03 Apr
08/10 Apr
15/17 Apr
22/24 Apr
29/01 Apr
06/08 May


### assignments we have so far

setup: needs editing of text to combine with old E0, maybe remove VM
new_syscall
scavenger_hunt
ctf
new_module
shell: nolibc?
specification
open_source_software


proposed assignments:
* Simulated device to create a device driver for

### Down the road:
* our own OS
* double compat userspace
