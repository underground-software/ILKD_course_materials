---
<!--
theme: default
_backgroundColor: black
_color: white
-->
---

# Barebones Linux

---

# Lecture overview

0. Why [setup](/assignments/setup.md)?

0. Barebones kernel

0. Using GDB with the kernel

0. Printing to the console

0. Device Tree

0. Basic Debugging

---

# Why setup

* Understand how small the kernel can be

* See how easy building the kernel can be

* Setup your development environment

---

# Refresher: What is the Linux Kernel?

Linux vs Linux

An ecosystem and a kernel

---

# Refresher: What is the Linux Kernel?

Core component of Linux that manages communication between hardware and software.

---

# Refresher: What is the Linux Kernel?

Responsible for:

* Resource management

* Process control

* Device drivers

* Security

* More!

---

# What is a config file?

Could have over 4000 options enabled by default!

---

# Start from scratch

---

# The Barebones Kernel

What is the smallest config possible?

* Pros

* Cons

---

# The Smallest Config

```
# Allow turning off even more stuff
CONFIG_EXPERT=y
CONFIG_NONPORTABLE=y

# Save time by skipping compressing the kernel, we only want an uncompressed image for qemu
CONFIG_KERNEL_UNCOMPRESSED=y

# Build a kernel to run in m-mode so we can avoid needing a bios
CONFIG_RISCV_M_MODE=y
```
---

# Build the config

```
ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make KCONFIG_ALLCONFIG=no.config allnoconfig
```
---

# Command Breakdown

* `ARCH=riscv`: specify non-host architecture to kernel build system ([Kbuild](https://docs.kernel.org/kbuild/kbuild.html))

* `CROSS_COMPILE=riscv64-linux-gnu-`: specify prefix for `gcc` command in order to invoke cross-compiler

* `make`: invoke the make program (the real command)

* `KCONFIG_ALLCONFIG=no.config`: generate a full config using this file as a starting point

* `allnoconfig`: the makefile target to generate a config with everything set to no unless specified in `no.config`

---

# Cross compile the kernel

```
ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc)
```

---

# Command Breakdown

* `ARCH=riscv`: specify non-host architecture to kernel build system ([Kbuild](https://docs.kernel.org/kbuild/kbuild.html))

* `CROSS_COMPILE=riscv64-linux-gnu-`: specify prefix for `gcc` command in order to invoke cross-compiler

* `make`: invoke the make program (the real command)

* `-j $(nproc)`: perform a multi-threaded build, creating as many threads as there are visible CPU cores on the system

* No target is specified, therefore `make` falls back to the default target, i.e. build the kernel (and generate `arch/riscv/boot/Image`)

---

# Launch the emulator

```
qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image
```

---

# Command Breakdown

* `qemu-system-riscv64`: invoke the RISC-V 64-bit emulator

* `-machine virt`: specify a hardware configuration suitable for general purpose virtualization

* `-bios none`: disable bios loading (and later directly specify a kernel binary image)

* `-nographic`: disable graphical output

* `-no-reboot`: don't automatically reboot the OS exits or errors

* `-net none`: disable networking

* `-kernel arch/riscv/boot/Image`: assuming we invoke this within the kernel repo post-build, pass the resulting binary to QEMU

---


# How do we see what's going on?

---

# GDB

---

# A `gdbinit` file

```
# disable confirmation prompts to save time (for kill/quit)
set confirm off

# automatically disassemble the next line when stepping through code
set disassemble-next-line auto

# set arch to RISC-V (64-bit variant)
set architecture riscv:rv64

# load the symbol table from the vmlinux file
symbol-file vmlinux

# load the vmlinux file for debugging
file vmlinux

# connect to the remote host at localhost on port 1234
target remote localhost:1234
```
---

# Relaunch the emulator

Why does it hang?

```
qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image -S -s
```

---

# Breakdown of new arguments:

* `-s`: enable debug socket on port 1234
* `-S`: freeze the CPU and wait for continue from debugger before running any instructions

---

# In a separate shell session

```
gdb -x gdbinit
... c to continue
... Ctrl+C to send interrupt
... bt to backtrace
...................
... kill to stop the machine
```
---

# Puzzling...

How do we make this more detailed?

Where are the line numbers?

---

# Enable debuginfo in .config

```
# Include debug symbols
CONFIG_DEBUG_INFO_DWARF4=y
```

---

# Why did the kernel panic? 🤔

---

# Enable Printk & A Serial Console Device

```
... (trimmed)
# enable kernel printing output
CONFIG_PRINTK=y

# enable serial subsystem
CONFIG_TTY=y

# Add driver for serial device for machine
CONFIG_SERIAL_8250=y

# Allow using that serial port for console
CONFIG_SERIAL_8250_CONSOLE=y
```

---

# Run the new kernel!

---

# Silence...

Why?

* How does the kernel know to use this device?

---

# Another QEMU argument: `-append`

What do you think this does?

---

# Kernel command line arguments

```
earlycon=uart8250,mmio,0x10000000 console=uart8250,mmio,0x10000000
```

---

# Kernel command line arguments: Breakdown

* `{earlycon,console}=`: specify information needed to print to a console early in the boot process and then during normal execution

* `uart8250`: Use the 8250 UART driver that we enabled in the config

* `mmio`: The kernel should communicate with the driver using memory-mapped I/O rather than I/O ports

* `0x10000000`: Base address for the UART device in physical memory

[(console= vs earlycon= and friends)](https://docs.kernel.org/admin-guide/kernel-parameters.html)

---

# Let's run it!

---

What is the [admin guide](https://docs.kernel.org/admin-guide/init.html)?

---

# The Device Tree

Can we detect the device automatically?

---

# Introducing: `menuconfig`

A makefile target in Kbuild

---

# Enabling Device Tree for our serial device

```
ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make  menuconfig
  │     -> Device Drivers                                                                                                                                           │
  │       -> Character devices                                                                                                                                      │
  │         -> Enable TTY (TTY [=y])                                                                                                                                │
  │           -> Serial drivers                                                                                                                                     │
  │ (1)         -> Devicetree based probing for 8250 ports (SERIAL_OF_PLATFORM [=n])
... press y to enable
```

---

# How can we find this option more quickly?

---

# This is how

* '/' to search

* `CONFIG_SERIAL_OF_PLATFORM`

* press 1

* Save and exit

* New config is ready for compilation

---

# Drop `-append`

```
qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image
..... (panic)
```

---

# Debugging the Panic

Why are we panicing?

---

# I: Brain Dead

Give up and change majors

---

# II: Small Brain

Grep for text "no working init found"

How?

---

`grep -rnw -e <pattern>`

* `-r`: recursive search

* `-n`: display line number in file of results

* `-w`: only match full words

* `-i`: be case insensitive

* `-e`: specify pattern after this argument (optional if pattern is last argument)

---

# III: Small-medium brain

`git grep`

Optimized search using git's database

---

# IV: Medium Brain

Look for the function in the [source](https://elixir.bootlin.com/linux/v6.13/source/init/main.c#L1528)

---

# V: Big Brain

Use `addr2line` on address found in GDB output

```
addr2line <address> -e vmlinux
```

---
# VI: Galaxy Brain

Get good at GDB

---

# Run the kernel and interrupt it
```
c
...Ctrl+c
```

---

# Do a backtrace and select a frame
```
bt
frame <n>
```
---

# Disassemble the current function or list the source

```
disas
list
```
---

# Switch between various Text-user-interface formats
```
layout asm
layout src
layout next
tui focus next
tui disable
```

---

# Init

What is `try_to_run_init_process("/etc/init")`?

---

# Why does this fail?

---

# Note

Kernel is optimized during build so some code is skipped or inlined

---

# Interesting functions to explore

```
kernel_execve()

do_filep_open()
```

---

# Summary

- The essential functionality of the Linux kernel is quite minimal

---

# Summary

- Cross compiling the kernel can be relatively simple

---

# Summary

- The kernel is just another program that you can debug

---

# Summary

- Because the kernel is open source, you can read the complete source and documentation

---

# Summary

- GDB is a powerful and versatile tool with advanced features

---

# Summary

- The kernel is highly configurable and customizable

---

# Summary

- There is no magic in the Linux kernel; just engineering

---

# END
