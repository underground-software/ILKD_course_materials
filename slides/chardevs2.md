# chardevs2: the kernel whispers

---

# Learning objective

Deepen and enrich your understanding of character devices with a more concrete example

---

# Overview

1. Review the `kdlpdev` example from last week

     1. `open(2)`, `close(2)`, and `read(2)` implementations

1. Introduce `write(2)`, `ioctl(2)`, and `llseek(2)`

1. Introduce the `jiffies` counter and `HZ` constant

1. Discuss the device class system and `struct device`

1. Overview of MIDI and our simplification

1. Work through the design and implementation of `kkey`

1. Listen to the sound of the kernel

---

# Review

`kdlpdev.c`

1. Remember what we needed to make `cat` work?

1. We have to manually create the device file with `mknod(7)`

1. Annoying! Today we introduce an alternative

1. Guesses on what this might be?

---

# New syscalls

`write(2)`: reverse the polarity of `read(2)`

---

# `read(2)` and `write(2)` considerations

1. Make sure special cases of `count` value handled correctly

1. Properly advance the file position value

1. Take care to return sane error values if appropriate

---

# New syscalls

`ioctl(2)`: general purpose interface with a character device

1. Commonly used for device-specific operations

1. Originally used to control the terminal on earlier Unix systems

1. tty := teletype terminal

1. Historically controlled terminals

---

# unlocked_ioctl?

Kernel had a single lock long ago

1. IOCTL calls took the lock

1. Long since irrelevant

---

# IOCTL cmd/arg details

Kernel preferred way to define IOCTL commands with several macros

1. Can specify type of transferred data for validation

1. [kernel/ioctl.h](https://elixir.bootlin.com/linux/v6.13/source/arch/alpha/include/uapi/asm/ioctl.h#L48)

---

# IOCTL cmd example (AI generated)

1. Example ioctl cmd #defines from Claude:

```c
#include <linux/ioctl.h>
#define MYDEVICE_IOC_MAGIC  'k'
#define MYDEVICE_IOCRESET    _IO(MYDEVICE_IOC_MAGIC, 0)
#define MYDEVICE_IOCSQUALITY _IOW(MYDEVICE_IOC_MAGIC, 1, int)
#define MYDEVICE_IOCGQUALITY _IOR(MYDEVICE_IOC_MAGIC, 2, int)
```

---

# New syscalls

`lseek(2)`: change the file position pointer

Three common modes:

1. Offset from current value

1. Negative offset from end

1. Set value directly

All fairly boilerplate

---

# Compatibility quirk

Any guesses why we find `llseek` in file_operations instead of `lseek`?

* Hint: `llseek` is short for "long long seek"

* 32-bit and 64-bit compatibility

---

# Synchronizing with the CPU

1. The `jiffies` counts CPU timer interrupts since boot

1. Allows calculating elapsed time between in-kernel events

1. [jiffies defined](https://elixir.bootlin.com/linux/v6.13/source/include/linux/jiffies.h#L86)

1. [get_jiffies_64()](https://elixir.bootlin.com/linux/v6.13/source/include/linux/jiffies.h#L99)

---

# Synchronizing with the CPU

1. `HZ` kernel config value contains timer frequency

1. Units are in interrupts per second

1. [HZ](https://elixir.bootlin.com/linux/v6.13/source/kernel/Kconfig.hz)

---

# HZ and jiffies

1. `jiffies` unit is timer interrupts (since boot)

1. HZ unit is interrupts/seconds

1. Therefore: seconds since boot = jiffies / HZ

---

# Grouping devices

Linux provides a device class representation

1. Each device class groups devices of similar type

---

# Device class implementation

1. [struct class](https://elixir.bootlin.com/linux/v6.13/source/include/linux/device/class.h#L50)

1. [class_create()](https://elixir.bootlin.com/linux/v6.13/source/drivers/base/class.c#L266)

1. [class_destroy()](https://elixir.bootlin.com/linux/v6.13/source/drivers/base/class.c#L299)


---

# Device instance

The kernel provides a specific device instance representation

1. Can create these by hand

1. Can instantiate using a device class

1. Represents device instance directly

---

# Device instance vs device class

1. Device instance identified by major/minor pair

1. Device class identified by name string

1. Both provide a sysfs interface

---

# Devices vs character devices

A character device can provide an interface for a device instance

1. The device instance could alternatively provide a block device interface

---

# Device instance implementation

1. [struct device](https://elixir.bootlin.com/linux/v6.13/source/include/linux/device.h#L721)

1. [device_create()](https://elixir.bootlin.com/linux/v6.13/source/drivers/base/core.c#L4424)

    1. From class

1. [device_register()](https://elixir.bootlin.com/linux/v6.13/source/drivers/base/core.c#L3744)

    1. By hand

1. [device_destroy()](https://elixir.bootlin.com/linux/v6.13/source/drivers/base/core.c#L4487)

---

# demo

The least abstract demo so far this semester

---

![bg fit](/slides/kkey_pics/listen.jpeg)

---

# First step

What kind of audio format would be suitable?

* Regardless of anything said just now in class, we chose MIDI

---

# MIDI format overview

Encodes musical information in events

1. Rather than a stream of audio

---

# MIDI format overview

Series of event messages sent to MIDI device

1. Events processed as they're received

---

# MIDI messages

1. Event messages relating to music, e.g. press, release

1. Meta messages e.g. time signature, tempo, track names, lyrics

---

# MIDI time between events

1. Deltas placed in between messages

1. Deltas are variable-width, with 7 bits of information

1. Leading bit 1 until final byte to indicate end

---

# MIDI binary format outline

1. One global MIDI header

1. Some some number of track headers

1. A series of meta or event messages

    1. delimited by variable length delta values

---

![bg fit](https://www.file-recovery.com/signatures/MID.png)


---

# Further MIDI reading

1. <http://midi.teragonaudio.com>

1. <https://faydoc.tripod.com/formats/mid.htm>

---

![bg fit](/slides/kkey_pics/readwrite.jpeg)

---

# Music input scheme

Write system calls to one of the /dev/kkeyXXX files

Input value is either:

1. 0 - key release

1. 1 - key press

---

# Music output scheme

Use read system call on any /dev/kkeyXXX

1. Output data is valid MIDI file

1. Listen to the music

---

![bg fit](/slides/kkey_pics/simplified.jpeg)

---

# A couple more `kkey` details

1. Simplify format with single track and channel

1. An IOCTL cmd emptires the internal buffer

1. But how do we calculate the delta values?

---

![bg fit](/slides/kkey_pics/time.jpeg)

---

# An analogy

If the timer interrupt is the "heartbeat" of the kernel, then `kkey` reads it's pulse

1. This is not rigorous

---

# Optimization

We cache the MIDI file when it's generated

1. As long as we read again before writing, no need to re-generate

---

![bg fit](/slides/kkey_pics/cache.jpeg)

---

# Concerns, anyone?

Spot any flaws?

1. Think about this as we peruse the implementation

---


# Complete implementation

We will now tour [kkey.c](https://github.com/underground-software/ILKD_demos/blob/master/L14/kkey/kkey.c)

1. All course demos and more are now published to this repo

1. Any suggestions or fixes? Contributions welcome!

---

# demo

makefile usage:

```
make
make reset
make play
make clean
```

---

# demo

```
middle_c.sh
```

Note table at bottom of <https://faydoc.tripod.com/formats/mid.htm>

---

# demo

```
c_major.sh
```

---

# demo

A classic musical "Hello world" tune

```
twinkle_twinkle.c
```

---

# demo

A modified C418 song used in Minecraft soundtrack IIRC

```
microvenus.c
```

---

# Glaring issue number 1

Any guesses?

* No locking! (intentional)

---

# Other non-deterministic behavior

Any quesses?

* Each usage of `kkey` produces slightly different output

---

# Summary

The six entries of `struct file_operations` implemented by `kkey` are the complete set required for the remaining assignments in this course

---

# Summary

Using `struct device` and `struct class`, one can more easily group together and manage similar devices

---

# Summary

Using `jiffies` and `HZ`, we can calculate the passage of real wall clock time relative to system boot

---

# Summary

The interfaces provides to kernel modules allow for very flexible behavior and creative features

1. This module implements an interface similar to the final project

---

# End

Coming up soon: concurrency and a race-free `kkey`
