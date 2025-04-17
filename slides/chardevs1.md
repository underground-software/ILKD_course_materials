# chardevs: introduction

---

# Learning Objective

Understand the purpose and basic usage of a Linux character device

---

# Overview

1. What is a character device?

1. Why do we want to use character devices

1. How does the user interact with character devices?

1. How does a kernel module implement a character device?

---

# What is a character device?

Character by character kernel interface

---

# What is a character device?

Streamed, not buffered

---

# What is a character device?

Not necessarily related to physical device

1. Can implement a device driver

1. We will come back to this

---

# What is a character device?

A file in `/dev`

1. Identified by major and minor numbers

---

# What is a character device?

Generally implemented by a kernel module

* Why?

---

# Finding active character devices

Generally located in the `/dev` directory

---

# demo

`ls -l /dev`

1. `mknod(7)`

---

# demo

`cat /proc/devices`

---

# Interacting with character devices

Same interface as any other file

1. `cat`, `echo`, and other commands

1. System calls in a program

---

# demo

Playing with devices:

1. `/dev/null`: discard input

1. `/dev/zero`: zero output

1. `/dev/tty`: this terminal

1. `/dev/kmsg`: kernel ring buffer access

1. `/dev/urandom`: random bytes [(click for mythology)](https://www.2uo.de/myths-about-urandom/)

1. `/dev/mem`: physical memory access 😨

---

# Implementation demo design

Random number generator

1. Between 0 and 256

1. Read with `cat`

---

# Implementation  overview

1. Need major and minor numbers

1. Need file operations

1. Register device with cdev subsystem

1. Implement open and close handlers

1. Implement read and write handlers

---

# Starting point

1. Module stub

---

# Note on `__init`

1. Special section

1. Can reclaim after module load

---

# demo

`kdlpdev4.c`

---

# Choose major and minors

Contained in [`dev_t`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/types.h#L21) type

Related: [`MAJOR`, `MINOR`, and `MKDEV`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/kdev_t.h#L10)

1. We don't use these for now

---

# Choose major and minors

In `init()`: [`alloc_chrdev_region()`](https://elixir.bootlin.com/linux/v6.13/source/fs/char_dev.c#L236)

In `exit()`: [`unregister_chrdev_region()`](https://elixir.bootlin.com/linux/v6.13/source/fs/char_dev.c#L311)

---

# An alternative

[`register_chrdev_region()`](https://elixir.bootlin.com/linux/v6.13/source/fs/char_dev.c#L200)

1. Static rather than dynamic

1. Why don't we use this?

---

# demo

`kdlpdev3.c`

---

# Necessary structs

[`struct cdev`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/cdev.h#L14)

[`struct file_operations`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L2071)

---

# Register our character device

In `init()`: [`cdev_init()`](https://elixir.bootlin.com/linux/v6.13/source/fs/char_dev.c#L658) and [`cdev_add()`](https://elixir.bootlin.com/linux/v6.13/source/fs/char_dev.c#L479)

* Alternative: [`cdev_alloc()`](https://elixir.bootlin.com/linux/v6.13/source/fs/char_dev.c#L640)

* Why or why not?

In `exit()`: [`cdev_del()`](https://elixir.bootlin.com/linux/v6.13/source/fs/char_dev.c#L601)

---

# demo

`kdlpdev2.c`

---

# Implement open and close

[`struct inode`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L635)

[`struct file`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/fs.h#L1035)

---

# demo

`kdlpdev1.c`

---

# Implement read and write

[`copy_to_user()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/uaccess.h#L217)

`__user` macro

1. Useful for static analysis and documentation

1. No runtime effect

---

# demo

The complete `kdlpdev.c`

---

# Summary

A character device implements a character-by-character interface with the kernel

1. This is a common interface for device drivers

---

# Summary

A kernel module can implement a character device

---

# Summary

A character device is identified by a major and minor number

---

# Summary

Character devices are generally located in `/dev`

1. Not required

1. Only the major and minor matter to the kernel

---

# Summary

Character devices are labeled by 'c' in `ls -l` output

---

# Summary

The `/proc/devices` file contains a list of major numbers and character devices

---

# Summary

Use `open(2)`, `close(2)`, `read(2)`, `write(2)`, and more to interact with the character device

---

# End
