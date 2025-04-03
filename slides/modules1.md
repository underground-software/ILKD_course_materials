# modules: introduction

---

# Learning objective

Understand why and how to write a simple kernel module

---

# Overview

1. What is a kernel module?

1. Why write a kernel module?

1. Important considerations

1. How to load and unload

1. Building up an example

---

# Question

What is the difference between a kernel module and a device driver?

---

# What is a kernel module?

Definition: a kernel module is a chunk of code that can be loaded into the kernel at runtime

---

# What is a kernel module?

Modify the kernel without re-compiling or even rebooting

1. Written in C, but now can be written in rust

---

# What is a kernel module?

Built as a `*.ko` file

1. A "kernel object" in ELF format

---

# ELF sidenote

ELF: [Executable and linkable format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)

1. Unix standard for executable binaries and more

---

# demo

`file` on a module object file

---

# What a kernel module is *not*:

1. executable by itself, nor is it synchronous

1. safe, since all code runs directly in the kernel

1. a driver, rather it is a mechanism by which to implement a driver

---

# Why write a kernel module?

1. Keep the main kernel small

1. Quicker development process

1. Distribution flexibility

---

# Question

What could we use modules for?

---

# Common kernel module use-cases

1. Device drivers

1. Filesystem support e.g. `ext4`, `xfs`, and `nfs`

1. Network protocol support such as `ipv6`

1. Cryptographic protocols like `sha256` and `aes`

1. Virtualization module, e.g. KVM

---

# demo

```
lsmod
```
---

# demo

```
ls /lib/modules/$(uname -r)
```
---

# demo

```
/proc/modules
```

1. module_name: The name of the loaded module

1. size: Memory size used by the module (in bytes)

1. used_count: The number of processes using this module

1. dependencies: List of other modules that this module depends on (comma-separated)

1. state: Module state (Live, Loading, or Unloading)

1. offset: The kernel memory offset where the module is loaded

---

# Do we need modules?

* no

---

# Modules are an optional feature

Toggled in the compilation configuration

---

# demo

look at .config/menuconfig tri-state options

1. In kernel repo: `make help | grep all.*config`

---

# Multiple entry points

1. No `main()`, but `init()` runs on module load

1. Define `exit()` to be able to unload

1. Other entry points depend on module purpose

    1. e.g. `probe()`, members of `struct file_operations`

---

# Versioning

1. Modules must be compiled for specific kernel version

1. May access kernel symbol table

1. May export it's own symbols

---

# demo

[`EXPORT_SYMBOL`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/export.h#L68)

---

# In-tree vs out-of-tree

The tree in question: kernel repository

---

# In-tree

Build a module during kernel build process

1.  New modules require changes to kernel build system

---

# demo

Take a look at how new modules can be added to kernel build system

---

# Out-of-tree

Build modules as standalone units

1. Loading an out-of-tree (oot) module can mark the kernel as "tainted"

1. To install dependencies to build modules for current version:

    1. `sudo dnf install kernel-devel-$(uname -r)`

---

# Loading and unloading

3 main utilities:

1. `insmod`

1. `rmmod`

1. `modprobe`

---

# What is the difference?

1. `insmod` takes a path to a `*.ko` file

1. `modprobe` take a module name in the `/lib/modules/$(uname -r)` subtree

2. `rmmod` and `modprobe -r` takes the name of a loaded module


---

# Underlying system calls

1. `init_module(2)` loads a `struct module` into the kernel

1. `finit_module(2)` does the same thing with a file descriptor

1. `delete_module(2)` removes a module given it's name

---


# demo

ikconfig in the minimal distro: in-tree

1. See: `kernel/configs.c`

    1. plenty of stuff we will return to

1. Built as the module in the minimal linux demo rootfs

1. Show inserting/removing in different ways

1. Show `/proc/module` address in `/proc/kallsyms`

1. Show bits in `/sys/module`

---

# demo

The most minimal module possible

1. `MODULE_LICENSE("GPL")` is the only required meta information

---

# demo

minimal but removable module

---

# demo

print on enter and exit: A "Hello world" module

---

# demo

Take a look at [include/linux/module.h](https://elixir.bootlin.com/linux/v6.13/source/include/linux/module.h)

---

# Summary

Why and how to write a simple kernel module

---

# Summary

A Linux kernel module is an ELF object that can be loaded into a running kernel

---

# Summary

Modularization can reduce the kernel binary size and attack surface

---

# Summary

Use kernel modules to implement device drivers, filesystems, network protocols, and more

1. Can be built within or seperately from the rest of the kernel

---

# Summary

Multiple entry points, including `init()` which is run at load time

---

# Summary

Build using makefile provided by the kernel repository

---

# Summary

Load and unload using `modprobe`, `insmod`, and `rmmod`

1. Inspect loaded modules with `lsmod` or the `/proc/modules` file

---

# End
