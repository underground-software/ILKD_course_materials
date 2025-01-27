---
theme: white
---

# Configuring the Linux Kernel

---

# Refresher: What is the Linux Kernel
- Core component of Linux that manages communication between hardware and software.
- Responsible for:
  - Resource management
  - Process control
  - Device drivers
  - Security
- A typical `.config` for a modern kernel on x86_64 architecture could have over 4000
options enabled by default.

---

# What is a 'noconfig' kernel? Audience participation required
- Take a guess!
- What are the benefits of compiling such a kernel?
- What are the downsides?

---

# Demo: THE 'noconfig' kernel
- Access your kdlp_container: `podman run -it --rm --hostname kdlp --name kdlp -v kdlp_volume:/home kdlp_container`.
- In your `linux` directory, create the `no.config` with the following.
```
$ cat no.config
# Allow turning off even more stuff
CONFIG_EXPERT=y
CONFIG_NONPORTABLE=y

# Save time by skipping compressing the kernel, we only want an uncompressed image for qemu
CONFIG_KERNEL_UNCOMPRESSED=y

# Build a kernel to run in m-mode so we can avoid needing a bios
CONFIG_RISCV_M_MODE=y
```

---

# Demo: Compile and run
- Write your changes.
`ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make KCONFIG_ALLCONFIG=no.config allnoconfig`
- Cross-compile the 'noconfig' kernel for RISCV.
`ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc)`
- Access the QEMU with our new kernel running.
`qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image`
- What happened?

---

# Demo: Let's debug 😋
- Configure GDB on your host machine.
```
$ cat ~/gdbinit
set confirm off
set disassemble-next-line auto
set architecture riscv:rv64
symbol-file vmlinux
file vmlinux
target remote localhost:1234
```
- Run `git clone --depth=1 --branch v6.13 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git`
on your host Linux machine, if needed, and navigate to that directory.
- In two seperate terminals run:
  - `qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image -S -s`
  - `gdb -x ~/path/to/gdbinit`.
- GDB commands to try:
  - `c` to continue.
  - `Ctrl+c` to send interrupt.
  - `bt` to backtrace
- What do you notice? Why did this happen?
- Add debug mode GDB.

---

# Demo: More debugging 😩
- Enable serial output in the kernel in order to use printk.
```
$ cat noise.config
 Allow turning off even more stuff
CONFIG_EXPERT=y
CONFIG_NONPORTABLE=y

# Save time by skipping compressing the kernel, we only want an uncompressed image for qemu
CONFIG_KERNEL_UNCOMPRESSED=y

# Build a kernel to run in m-mode so we can avoid needing a bios
CONFIG_RISCV_M_MODE=y

# enable kernel printing output
CONFIG_PRINTK=y

# enable serial subsystem
CONFIG_TTY=y

# Add driver for serial device for machine
CONFIG_SERIAL_8250=y

# Allow using that serial port for console
CONFIG_SERIAL_8250_CONSOLE=y
```
- Write configuration, compile, and run.
`ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make KCONFIG_ALLCONFIG=noise.config allnoconfig &&
ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc) &&
qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image`
- What is happening?
- Try `qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image -append`

---

# Demo: Useful links on how we continue
- [Explaining the “No working init found.” boot hang message](https://docs.kernel.org/admin-guide/init.html)
- [The kernel’s command-line parameters](https://docs.kernel.org/admin-guide/kernel-parameters.html)
- Where is the panic message coming from?
  - [Hint](https://elixir.bootlin.com/linux/v6.13/source/init/main.c#L1528).
