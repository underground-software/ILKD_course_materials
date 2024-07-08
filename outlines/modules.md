# modules

1. Learning objective: understand why and how to write a simple kernel module

1. Overview

    1. What is a kernel module?

    1. Why write a kernel module?

    1. Important considerations

    1. How to load and unload

    1. Building up an example

1. What is a kernel module?

    1. A chunk of code that can be loaded into the kernel at runtime

        1. Modify the kernel without re-compiling or even rebooting

    1. Written in C, but now can be written in rust

    1. Built as a `*.ko` file, a "kernel object" in ELF format

        1. demo: `file` on a module object file

    1. what a kernel module is *not*:

        1. executable by itself, nor is it synchronous

        1. safe, since all code runs directly in the kernel

        1. a driver, rather it is a mechanism by which to implement a driver

1. Why write a kernel module?

    1. Keep the main kernel small

    1. Add some functionality to the kernel

        1. Device drivers

        1. Filesystem support e.g. `ext4`, `xfs`, and `nfs`

        1. Network protocol support such as `ipv6`

        1. Cryptographic protocols like `sha256` and `aes`

        1. Virtualization module, e.g. KVM

1. demo: lsmod

1. demo: ls /lib/modules/$(uname -r)

1. demo: /proc/modules

    1. module_name: The name of the loaded module

    1. size: Memory size used by the module (in bytes)

    1. used_count: The number of processes using this module

    1. dependencies: List of other modules that this module depends on (comma-separated)

    1. state: Module state (Live, Loading, or Unloading)

    1. offset: The kernel memory offset where the module is loaded

1. Important considerations

    1. The kernel can run without modules

        1. We can compile modules into the kernel

        1. demo: look at .config/menuconfig tri-state options

            1. In kernel repo: make help | grep all.*config

    1. Multiple entry points

        1. No `main()`, but `init()` runs on module load

        1. Define `exit()` to be able to unload

        1. Other entry points depend on module purpose

            1. e.g. `probe()`, members of `struct file_operations`

    1. Versioning

        1. Modules must be compiled for specific kernel version

        1. May access kernel symbol table

        1. May export it's own symbols

    1. In-tree vs out-of-tree

        1. Can build modules standalone: out-of-tree

            1. Loading an out-of-tree (oot) module can mark the kernel as "tainted"

            1. To install dependencies to build modules for current version:

                1. `sudo dnf install kernel-devel-$(uname -r)`

        1. Can build modules as part of kernel build: in-tree

            1. New modules require changes to kernel build system

            1. demo: take a look at how modules can be added ot kernel build system

1. How to load and unload modules

    1. CLI utilities

        1. insmod, rmmod, and modprobe

        1. {ins,rm}mod take a path to a `*.ko` file

        1. modprobe {,-r} takes a module name within /lib/modules/$(uname -r)

    1. Underlying system calls

        1. The utilities are built on two system calls

        1. `init_module(2)` loads a `struct module` into the kernel

        1. `finit_module(2)` does the same thing with a file descriptor

        1. `delete_module(2)` removes a module given it's name

    1. Kernel recieves the request

        1. Module loaded from disk and linked into kernel

1. demo: ikconfig

    1. See: kernel/configs.c

        1. plenty of stuff we will return to

    1. built as only module in minimal linux demo

    1. show inserting/removing in different ways

    1. show /proc/module address in /proc/kallsyms

    1. show bits in /sys/module

1. Building up an example

    1. demo: minimal, non-removable

    1. demo: removable

    1. demo: print on enter and exit

    1. `MODULE_LICENSE("GPL")` is the only required meta information

    1. demo: Take a look at [include/linux/module.h](https://elixir.bootlin.com/linux/v6.5/source/include/linux/module.h#L230)

1. Summary: Why and how to write a simple kernel module

    1. A Linux kernel module is an ELF object that can be loaded into a running kernel

    1. Modularization can reduce the kernel binary size and attack surface

    1. Implements device drivers, filesystems, network protocols, and more

    1. Can be built within or seperately from the rest of the kernel

    1. Multiple entry points, including `init()` which is run at load time

    1. Build using makefile provided by the kernel repository

    1. Load and unload using `modprobe`, `insmod`, and `rmmod`

    1. Inspect loaded modules with `lsmod` or the `/proc/modules` file
